--TEST--
DDTrace\trace_function() can trace with internal spans
--ENV--
DD_TRACE_GENERATE_ROOT_SPAN=0
DD_TRACE_TRACED_INTERNAL_FUNCTIONS=array_sum,mt_rand
--FILE--
<?php
use DDTrace\SpanData;

function testFoo()
{
    echo "testFoo()\n";
}

function addOne($num)
{
    echo "addOne()\n";
    return $num + 1;
}

function bar($thoughts, array $bar = [])
{
    echo "bar()\n";
    return [
        'thoughts' => $thoughts,
        'first' => isset($bar[0]) ? $bar[0] : '(none)',
        'rand' => array_sum([
            mt_rand(0, 100),
            addOne(mt_rand(0, 100)),
        ])
    ];
}

var_dump(DDTrace\trace_function('array_sum', function (SpanData $span) {
    $span->name = 'ArraySum';
}));
var_dump(DDTrace\trace_function('mt_rand', null));
var_dump(DDTrace\trace_function('testFoo', function (SpanData $span) {
    $span->name = 'TestFoo';
}));
var_dump(DDTrace\trace_function('addOne', function (SpanData $span) {
    $span->name = 'AddOne';
}));
var_dump(DDTrace\trace_function(
    'bar',
    function (SpanData $span, $args, $retval) {
        $span->name = 'BarName';
        $span->resource = 'BarResource';
        $span->service = 'BarService';
        $span->type = 'BarType';
        $span->meta += [
            'args.0' => isset($args[0]) ? $args[0] : '',
            'retval.thoughts' => isset($retval['thoughts']) ? $retval['thoughts'] : '',
            'retval.first' => isset($retval['first']) ? $retval['first'] : '',
            'retval.rand' => isset($retval['rand']) ? $retval['rand'] : '',
        ];
        $span->metrics += [
            'foo' => isset($args[1][1]) ? $args[1][1] : '',
            'bar' => isset($args[1][2]) ? $args[1][2] : '',
        ];
    }
));

testFoo();
var_dump(addOne(0));
$ret = bar('tracing is awesome', ['first', 1.2, '25']);
var_dump($ret);

echo "---\n";

var_dump(dd_trace_serialize_closed_spans());
var_dump(dd_trace_serialize_closed_spans());
?>
--EXPECTF--
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
testFoo()
addOne()
int(1)
bar()
addOne()
array(3) {
  ["thoughts"]=>
  string(18) "tracing is awesome"
  ["first"]=>
  string(5) "first"
  ["rand"]=>
  int(%d)
}
---
array(5) {
  [0]=>
  array(10) {
    ["trace_id"]=>
    string(%d) "%d"
    ["span_id"]=>
    string(%d) "%d"
    ["start"]=>
    int(%d)
    ["duration"]=>
    int(%d)
    ["name"]=>
    string(7) "BarName"
    ["resource"]=>
    string(11) "BarResource"
    ["service"]=>
    string(10) "BarService"
    ["type"]=>
    string(7) "BarType"
    ["meta"]=>
    array(6) {
      ["system.pid"]=>
      string(%d) "%d"
      ["args.0"]=>
      string(18) "tracing is awesome"
      ["retval.thoughts"]=>
      string(18) "tracing is awesome"
      ["retval.first"]=>
      string(5) "first"
      ["retval.rand"]=>
      string(%d) "%d"
      ["_dd.p.upstream_services"]=>
      string(24) "QmFyU2VydmljZQ|1|1|1.000"
    }
    ["metrics"]=>
    array(5) {
      ["foo"]=>
      float(1.2)
      ["bar"]=>
      float(25)
      ["_dd.rule_psr"]=>
      float(1)
      ["_sampling_priority_v1"]=>
      float(1)
      ["php.compilation.total_time_ms"]=>
      float(%f)
    }
  }
  [1]=>
  array(9) {
    ["trace_id"]=>
    string(%d) "%d"
    ["span_id"]=>
    string(%d) "%d"
    ["parent_id"]=>
    string(%d) "%d"
    ["start"]=>
    int(%d)
    ["duration"]=>
    int(%d)
    ["name"]=>
    string(8) "ArraySum"
    ["resource"]=>
    string(8) "ArraySum"
    ["service"]=>
    string(29) "dd_trace_function_complex.php"
    ["type"]=>
    string(3) "cli"
  }
  [2]=>
  array(9) {
    ["trace_id"]=>
    string(%d) "%d"
    ["span_id"]=>
    string(%d) "%d"
    ["parent_id"]=>
    string(%d) "%d"
    ["start"]=>
    int(%d)
    ["duration"]=>
    int(%d)
    ["name"]=>
    string(6) "AddOne"
    ["resource"]=>
    string(6) "AddOne"
    ["service"]=>
    string(29) "dd_trace_function_complex.php"
    ["type"]=>
    string(3) "cli"
  }
  [3]=>
  array(10) {
    ["trace_id"]=>
    string(%d) "%d"
    ["span_id"]=>
    string(%d) "%d"
    ["start"]=>
    int(%d)
    ["duration"]=>
    int(%d)
    ["name"]=>
    string(6) "AddOne"
    ["resource"]=>
    string(6) "AddOne"
    ["service"]=>
    string(29) "dd_trace_function_complex.php"
    ["type"]=>
    string(3) "cli"
    ["meta"]=>
    array(2) {
      ["system.pid"]=>
      string(%d) "%d"
      ["_dd.p.upstream_services"]=>
      string(49) "ZGRfdHJhY2VfZnVuY3Rpb25fY29tcGxleC5waHA|1|1|1.000"
    }
    ["metrics"]=>
    array(3) {
      ["_dd.rule_psr"]=>
      float(1)
      ["_sampling_priority_v1"]=>
      float(1)
      ["php.compilation.total_time_ms"]=>
      float(%f)
    }
  }
  [4]=>
  array(10) {
    ["trace_id"]=>
    string(%d) "%d"
    ["span_id"]=>
    string(%d) "%d"
    ["start"]=>
    int(%d)
    ["duration"]=>
    int(%d)
    ["name"]=>
    string(7) "TestFoo"
    ["resource"]=>
    string(7) "TestFoo"
    ["service"]=>
    string(29) "dd_trace_function_complex.php"
    ["type"]=>
    string(3) "cli"
    ["meta"]=>
    array(2) {
      ["system.pid"]=>
      string(%d) "%d"
      ["_dd.p.upstream_services"]=>
      string(49) "ZGRfdHJhY2VfZnVuY3Rpb25fY29tcGxleC5waHA|1|1|1.000"
    }
    ["metrics"]=>
    array(3) {
      ["_dd.rule_psr"]=>
      float(1)
      ["_sampling_priority_v1"]=>
      float(1)
      ["php.compilation.total_time_ms"]=>
      float(%f)
    }
  }
}
array(0) {
}
