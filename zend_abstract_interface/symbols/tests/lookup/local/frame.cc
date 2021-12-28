extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"
#include "zai_sapi/zai_sapi_extension.h"

#include "zai_compat.h"

static zval* ddtrace_testing_frame_result;

ZEND_BEGIN_ARG_INFO_EX(ddtrace_testing_frame_intercept_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

// clang-format off
static PHP_FUNCTION(ddtrace_testing_frame_intercept) {
#if PHP_VERSION_ID >= 70000
    zend_execute_data* frame = EX(prev_execute_data);
#else
    zend_execute_data* frame =
        EG(current_execute_data)
            ->prev_execute_data;
#endif
    zai_string_view name = ZAI_STRL_VIEW("var");

    zval* result = (zval*) zai_symbol_lookup(
        ZAI_SYMBOL_TYPE_LOCAL,
        ZAI_SYMBOL_SCOPE_FRAME,
        frame, &name ZAI_TSRMLS_CC);

    ZAI_VALUE_COPY(ddtrace_testing_frame_result, result);

    RETURN_NULL();
}

static zend_function_entry ddtrace_testing_frame_extension_functions[] = {
    PHP_FE(ddtrace_testing_frame_intercept, ddtrace_testing_frame_intercept_arginfo)
    PHP_FE_END
};

static zend_module_entry ddtrace_testing_frame_extension = {
    STANDARD_MODULE_HEADER,
    "DDTraceTestingSymbolsLocalFrame",
    ddtrace_testing_frame_extension_functions,  // Functions
    NULL,  // MINIT
    NULL,  // MSHUTDOWN
    NULL,  // RINIT
    NULL,  // RSHUTDOWN
    NULL,  // Info function
    PHP_VERSION,
    STANDARD_MODULE_PROPERTIES
};
// clang-format on
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/lookup/local/frame]") { \
        REQUIRE(zai_sapi_sinit()); \
        zai_module.php_ini_ignore = 1; \
        zai_sapi_extension = ddtrace_testing_frame_extension; \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        zai_sapi_execute_script("./stubs/lookup/local/frame/Stub.php"); \
        ZAI_VALUE_MAKE(ddtrace_testing_frame_result); \
        { code } \
        ZAI_VALUE_DTOR(ddtrace_testing_frame_result); \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITH_STUB("symbol/lookup/local/frame [scalar]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view name = ZAI_STRL_VIEW("scalar");
    zend_function* method = (zend_function*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_KNOWN, method, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(ddtrace_testing_frame_result) == IS_LONG);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/lookup/local/frame [refcounted]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view name = ZAI_STRL_VIEW("refcounted");
    zend_function* method = (zend_function*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_KNOWN, method, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(ddtrace_testing_frame_result) == IS_OBJECT);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/lookup/local/frame [reference]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view name = ZAI_STRL_VIEW("reference");
    zend_function* method = (zend_function*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_KNOWN, method, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(ddtrace_testing_frame_result) == IS_OBJECT);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/lookup/local/frame [param]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *result;
    ZAI_VALUE_INIT(result);

    zval *param;
    ZAI_VALUE_MAKE(param);

    ZVAL_LONG(param, 42);

    zai_string_view name = ZAI_STRL_VIEW("param");
    zend_function* method = (zend_function*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_KNOWN, method, &result ZAI_TSRMLS_CC, 1, &param));

    REQUIRE(Z_TYPE_P(ddtrace_testing_frame_result) == IS_LONG);

    ZAI_VALUE_DTOR(result);
    ZAI_VALUE_DTOR(param);
})

