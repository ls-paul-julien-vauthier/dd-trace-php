<?php

namespace DDTrace\Tests\Unit;

use DDTrace\Format;
use DDTrace\Sampling\PrioritySampling;
use DDTrace\SpanContext;
use DDTrace\Tag;
use DDTrace\Tests\DebugTransport;
use DDTrace\Time;
use DDTrace\Tracer;
use DDTrace\Transport\Noop as NoopTransport;
use DDTrace\Tests\Common\BaseTestCase;

function baz()
{
}

final class TracerTest extends BaseTestCase
{
    const OPERATION_NAME = 'test_span';
    const ANOTHER_OPERATION_NAME = 'test_span2';
    const TAG_KEY = 'test_key';
    const TAG_VALUE = 'test_value';
    const FORMAT = 'test_format';

    protected function ddSetUp()
    {
        self::putenv('DD_AUTOFINISH_SPANS');
        self::putenv('DD_TRACE_REPORT_HOSTNAME');
        self::putenv('DD_TAGS');
        parent::ddSetUp();
    }

    protected function ddTearDown()
    {
        parent::ddTearDown();
        self::putenv('DD_TRACE_REPORT_HOSTNAME');
        self::putenv('DD_AUTOFINISH_SPANS');
        self::putenv('DD_TAGS');
    }

    public function testStartSpanAsNoop()
    {
        $tracer = Tracer::noop();
        $span = $tracer->startSpan(self::OPERATION_NAME);
        $this->assertInstanceOf('DDTrace\NoopSpan', $span);
    }

    public function testCreateSpanSuccessWithExpectedValues()
    {
        $tracer = new Tracer(new NoopTransport());
        $tracer->startRootSpan('foo'); // setting start_time not allowed on internal root span
        $startTime = Time::now();
        $span = $tracer->startSpan(self::OPERATION_NAME, [
            'tags' => [
                self::TAG_KEY => self::TAG_VALUE
            ],
            'start_time' => $startTime,
            'child_of' => $tracer->getActiveSpan(),
        ]);

        $this->assertEquals(self::OPERATION_NAME, $span->getOperationName());
        $this->assertEquals(self::TAG_VALUE, $span->getTag(self::TAG_KEY));
        $this->assertEquals($startTime, $span->getStartTime());
    }

    public function testStartSpanAsChild()
    {
        $context = SpanContext::createAsRoot();
        $tracer = new Tracer(new NoopTransport());
        $span = $tracer->startSpan(self::OPERATION_NAME, [
            'child_of' => $context,
        ]);
        $this->assertEquals($context->getSpanId(), $span->getParentId());
        $this->assertNull($span->getTag(Tag::PID));
    }

    public function testStartSpanAsRootWithPid()
    {
        \dd_trace_serialize_closed_spans();
        $tracer = new Tracer(new NoopTransport());
        $span = $tracer->startSpan(self::OPERATION_NAME);
        $this->assertEquals(getmypid(), $span->getTag(Tag::PID));
    }

    public function testStartActiveSpan()
    {
        $tracer = new Tracer(new NoopTransport());
        $scope = $tracer->startActiveSpan(self::OPERATION_NAME);
        $this->assertEquals($scope, $tracer->getScopeManager()->getActive());
    }

    public function testStartActiveSpanAsChild()
    {
        $tracer = new Tracer(new NoopTransport());
        $parentScope = $tracer->startActiveSpan(self::OPERATION_NAME);
        $parentSpan = $parentScope->getSpan();
        $parentSpan->setTag(Tag::SERVICE_NAME, 'parent_service');
        $childScope = $tracer->startActiveSpan(self::ANOTHER_OPERATION_NAME);
        $this->assertEquals($childScope, $tracer->getScopeManager()->getActive());
        $this->assertEquals($parentScope->getSpan()->getSpanId(), $childScope->getSpan()->getParentId());
        $this->assertEquals($parentScope->getSpan()->getService(), $childScope->getSpan()->getService());
    }

    public function testInjectThrowsUnsupportedFormatException()
    {
        $this->setExpectedException('\DDTrace\Exceptions\UnsupportedFormat');
        $context = SpanContext::createAsRoot();
        $carrier = [];

        $tracer = new Tracer(new NoopTransport());
        $tracer->inject($context, self::FORMAT, $carrier);
    }

    public function testInjectCallsTheRightInjector()
    {
        $context = SpanContext::createAsRoot();
        $carrier = [];

        $propagator = $this->prophesize('DDTrace\Propagator');
        $propagator->inject($context, $carrier)->shouldBeCalled();
        $tracer = new Tracer(new NoopTransport(), [self::FORMAT => $propagator->reveal()]);
        $tracer->inject($context, self::FORMAT, $carrier);
    }

    public function testExtractThrowsUnsupportedFormatException()
    {
        $this->setExpectedException('\DDTrace\Exceptions\UnsupportedFormat');
        $carrier = [];
        $tracer = new Tracer(new NoopTransport());
        $tracer->extract(self::FORMAT, $carrier);
    }

    public function testExtractCallsTheRightExtractor()
    {
        $expectedContext = SpanContext::createAsRoot();
        $carrier = [];

        $propagator = $this->prophesize('DDTrace\Propagator');
        $propagator->extract($carrier)->shouldBeCalled()->willReturn($expectedContext);
        $tracer = new Tracer(new NoopTransport(), [self::FORMAT => $propagator->reveal()]);
        $actualContext = $tracer->extract(self::FORMAT, $carrier);
        $this->assertEquals($expectedContext, $actualContext);
    }

    public function testPrioritySamplingIsEarlyAssignedAndRefreshedOnInject()
    {
        $tracer = new Tracer(new DebugTransport());
        $span = $tracer->startRootSpan(self::OPERATION_NAME)->getSpan();
        $this->assertSame(PrioritySampling::USER_KEEP, $tracer->getPrioritySampling());
        $span->metrics = [];
        $carrier = [];
        $tracer->inject($span->getContext(), Format::TEXT_MAP, $carrier);
        $this->assertSame(PrioritySampling::USER_KEEP, $tracer->getPrioritySampling());
    }

    public function testPrioritySamplingIsLazilyAssignedAndRefreshedBeforeFlush()
    {
        $tracer = new Tracer(new DebugTransport());
        $span = $tracer->startRootSpan(self::OPERATION_NAME)->getSpan();
        $this->assertSame(PrioritySampling::USER_KEEP, $tracer->getPrioritySampling());
        $span->metrics = [];
        $this->assertSame(PrioritySampling::USER_KEEP, $tracer->getPrioritySampling());
    }

    public function testPrioritySamplingInheritedFromDistributedTracingContext()
    {
        $distributedTracingContext = new SpanContext('', '', '', [], true);
        $distributedTracingContext->setPropagatedPrioritySampling(PrioritySampling::USER_REJECT);
        $tracer = new Tracer(new DebugTransport());
        $tracer->startRootSpan(self::OPERATION_NAME, [
            'child_of' => $distributedTracingContext,
        ]);
        // We need to flush as priority sampling is lazily evaluated at inject time or flush time.
        $tracer->flush();
        $this->assertSame(PrioritySampling::USER_REJECT, $tracer->getPrioritySampling());
    }

    public function testSpanStartedAtRootCanBeAccessedLater()
    {
        $tracer = new Tracer(new NoopTransport());
        $scope = $tracer->startRootSpan(self::OPERATION_NAME);
        $this->assertSame($scope, $tracer->getRootScope());
    }

    public function testFlushAddsHostnameToRootSpanWhenEnabled()
    {
        self::putenv('DD_TRACE_REPORT_HOSTNAME=true');

        \dd_trace_serialize_closed_spans();
        $tracer = new Tracer(new NoopTransport());
        $scope = $tracer->startRootSpan(self::OPERATION_NAME);
        $this->assertEquals(gethostname(), $tracer->getRootScope()->getSpan()->getTag(Tag::HOSTNAME));
    }

    public function testIfNoRootScopeExistsItWillBeNull()
    {
        $tracer = new Tracer(new NoopTransport());
        $this->assertNull($tracer->getRootScope());
    }

    public function testHonorGlobalTags()
    {
        self::putenv('DD_TAGS=key1:value1,key2:value2');

        \dd_trace_serialize_closed_spans();
        $transport = new DebugTransport();
        $tracer = new Tracer($transport);
        $span = $tracer->startSpan('custom');

        $this->assertSame('value1', $span->getAllTags()['key1']);
        $this->assertSame('value2', $span->getAllTags()['key2']);

        self::putenv('DD_TAGS='); // prevent memory leak
    }

    public function testInternalAndUserlandSpansAreMergedIntoSameTraceOnSerialization()
    {
        self::putenv('DD_TRACE_GENERATE_ROOT_SPAN=0');
        dd_trace_internal_fn('ddtrace_reload_config');

        // Clear existing internal spans
        \dd_trace_serialize_closed_spans();

        \DDTrace\trace_function(__NAMESPACE__ . '\\baz', function () {
            // Do nothing
        });
        $tracer = new Tracer(new DebugTransport());

        $tracer->startActiveSpan('bar');
        baz();
        $tracer->getActiveSpan()->finish();

        $this->assertSame(2, dd_trace_closed_spans_count());
        $traces = \dd_trace_serialize_closed_spans();
        $this->assertCount(2, $traces);

        self::putenv('DD_TRACE_GENERATE_ROOT_SPAN');
        dd_trace_internal_fn('ddtrace_reload_config');
    }
}
