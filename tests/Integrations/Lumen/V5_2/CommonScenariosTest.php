<?php

namespace DDTrace\Tests\Integrations\Lumen\V5_2;

use DDTrace\Tests\Common\SpanAssertion;
use DDTrace\Tests\Common\WebFrameworkTestCase;
use DDTrace\Tests\Frameworks\Util\Request\RequestSpec;

class CommonScenariosTest extends WebFrameworkTestCase
{
    protected static function getAppIndexScript()
    {
        return __DIR__ . '/../../../Frameworks/Lumen/Version_5_2/public/index.php';
    }

    protected static function getEnvs()
    {
        return array_merge(parent::getEnvs(), [
            'DD_SERVICE' => 'lumen_test_app',
        ]);
    }

    /**
     * @dataProvider provideSpecs
     * @param RequestSpec $spec
     * @param array $spanExpectations
     * @throws \Exception
     */
    public function testScenario(RequestSpec $spec, array $spanExpectations)
    {
        $traces = $this->tracesFromWebRequest(function () use ($spec) {
            $this->call($spec);
        });

        $this->assertFlameGraph($traces, $spanExpectations);
    }

    public function provideSpecs()
    {
        return $this->buildDataProvider(
            [
                'A simple GET request returning a string' => $this->getSimpleTrace(),
                'A simple GET request with a view' => [
                    SpanAssertion::build(
                        'lumen.request',
                        'lumen_test_app',
                        'web',
                        'GET /simple_view'
                    )->withExactTags([
                        'lumen.route.action' => 'App\Http\Controllers\ExampleController@simpleView',
                        'http.method' => 'GET',
                        'http.url' => 'http://localhost:9999/simple_view',
                        'http.status_code' => '200',
                    ])->withChildren([
                        SpanAssertion::build(
                            'Laravel\Lumen\Application.handleFoundRoute',
                            'lumen_test_app',
                            'web',
                            'Laravel\Lumen\Application.handleFoundRoute'
                        )->withExactTags([
                            'lumen.route.action' => 'App\Http\Controllers\ExampleController@simpleView',
                        ])
                        ->withChildren([
                            SpanAssertion::build(
                                'laravel.view.render',
                                'lumen_test_app',
                                'web',
                                'simple_view'
                            )->withExactTags([])
                            ->withChildren([
                                SpanAssertion::build(
                                    'lumen.view',
                                    'lumen_test_app',
                                    'web',
                                    '*/resources/views/simple_view.blade.php'
                                )->withExactTags([]),
                                SpanAssertion::build(
                                    'laravel.event.handle',
                                    'lumen_test_app',
                                    'web',
                                    'composing: simple_view'
                                )->withExactTags([]),
                            ]),
                            SpanAssertion::build(
                                'laravel.event.handle',
                                'lumen_test_app',
                                'web',
                                'creating: simple_view'
                            )->withExactTags([])
                        ])
                    ]),
                ],
                'A GET request with an exception' => [
                    SpanAssertion::build(
                        'lumen.request',
                        'lumen_test_app',
                        'web',
                        'GET /error'
                    )->withExactTags([
                        'lumen.route.action' => 'App\Http\Controllers\ExampleController@error',
                        'http.method' => 'GET',
                        'http.url' => 'http://localhost:9999/error',
                        'http.status_code' => '500',
                    ])->withExistingTagsNames(\PHP_MAJOR_VERSION === 5 ? [] : ['error.stack'])
                    ->setError(
                        \PHP_MAJOR_VERSION === 5 ? 'Internal Server Error' : 'Exception',
                        \PHP_MAJOR_VERSION === 5 ? null : 'Controller error'
                    )->withChildren([
                        SpanAssertion::build(
                            'Laravel\Lumen\Application.handleFoundRoute',
                            'lumen_test_app',
                            'web',
                            'Laravel\Lumen\Application.handleFoundRoute'
                        )->withExactTags([
                            'lumen.route.action' => 'App\Http\Controllers\ExampleController@error',
                        ])
                        ->withExistingTagsNames([
                            'error.stack'
                        ])->setError('Exception', 'Controller error'),
                        SpanAssertion::build(
                            'Laravel\Lumen\Application.sendExceptionToHandler',
                            'lumen_test_app',
                            'web',
                            'Laravel\Lumen\Application.sendExceptionToHandler'
                        ),
                    ]),
                ],
            ]
        );
    }

    protected function getSimpleTrace()
    {
        return [
            SpanAssertion::build(
                'lumen.request',
                'lumen_test_app',
                'web',
                'GET /simple'
            )->withExactTags([
                'lumen.route.name' => 'simple_route',
                'lumen.route.action' => 'App\Http\Controllers\ExampleController@simple',
                'http.method' => 'GET',
                'http.url' => 'http://localhost:9999/simple',
                'http.status_code' => '200',
            ])->withChildren([
                SpanAssertion::build(
                    'Laravel\Lumen\Application.handleFoundRoute',
                    'lumen_test_app',
                    'web',
                    'simple_route'
                )->withExactTags([
                    'lumen.route.action' => 'App\Http\Controllers\ExampleController@simple',
                ]),
            ]),
        ];
    }

    protected function getErrorTrace()
    {
        return [
            SpanAssertion::build(
                'lumen.request',
                'lumen_test_app',
                'web',
                'GET /error'
            )->withExactTags([
                'lumen.route.action' => 'App\Http\Controllers\ExampleController@error',
                'http.method' => 'GET',
                'http.url' => 'http://localhost:9999/error',
                'http.status_code' => '500',
            ])->setError()
                ->withChildren([
                    SpanAssertion::build(
                        'Laravel\Lumen\Application.handleFoundRoute',
                        'lumen_test_app',
                        '',
                        'Laravel\Lumen\Application.handleFoundRoute'
                    )->withExactTags([
                        'lumen.route.action' => 'App\Http\Controllers\ExampleController@error',
                    ])->withExistingTagsNames([
                        'error.stack'
                    ])->setError('Exception', 'Controller error'),
                ]),
        ];
    }
}
