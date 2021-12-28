extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITHOUT_STUB(name, code) TEST_CASE(name, "[symbol/lookup/constant]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/lookup/constant]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        zai_sapi_execute_script("./stubs/lookup/constant/Stub.php"); \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_USE(name, code) TEST_CASE(name, "[symbol/lookup/constant][.]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITHOUT_STUB("symbol/lookup constant [global, exists]", {
    zai_string_view name = ZAI_STRL_VIEW("PHP_VERSION");
    zai_string_view mixed = ZAI_STRL_VIEW("Php_Version");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup constant [global, does not exist]", {
    zai_string_view name = ZAI_STRL_VIEW("NO_SUCH_CONSTANT");
    zai_string_view mixed = ZAI_STRL_VIEW("No_Such_Constant");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup constant [root ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\");
    zai_string_view name = ZAI_STRL_VIEW("PHP_VERSION");
    zai_string_view mixed = ZAI_STRL_VIEW("Php_Version");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &name ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup constant [root ns fqn, exists]", {
    zai_string_view name = ZAI_STRL_VIEW("\\PHP_VERSION");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup constant [root ns, does not exist]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\");
    zai_string_view name  = ZAI_STRL_VIEW("NO_SUCH_CONSTANT");
    zai_string_view mixed = ZAI_STRL_VIEW("No_Such_Constant");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &name ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup constant [ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view name = ZAI_STRL_VIEW("DD_TRACE_TESTING");

    zval *constant = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &name ZAI_TSRMLS_CC);

    REQUIRE(constant);
    REQUIRE(Z_TYPE_P(constant) == IS_LONG);
    REQUIRE(Z_LVAL_P(constant) == 42);
})

TEST_WITH_STUB("symbol/lookup constant [ns, does not exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view name = ZAI_STRL_VIEW("DD_TEST_CONSTANT_DOES_NOT_EXIST");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup constant [class, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn   = ZAI_STRL_VIEW("Stub");
    zai_string_view name = ZAI_STRL_VIEW("DD_TRACE_TESTING");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zval *constant = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(constant);
    REQUIRE(Z_TYPE_P(constant) == IS_LONG);
    REQUIRE(Z_LVAL_P(constant) == 42);
})

TEST_WITH_STUB("symbol/lookup constant [class, does not exist]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn   = ZAI_STRL_VIEW("Stub");
    zai_string_view name = ZAI_STRL_VIEW("DD_TRACE_TESTING_DOES_NOT_EXIST");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup constant [object, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn   = ZAI_STRL_VIEW("Stub");
    zai_string_view name = ZAI_STRL_VIEW("DD_TRACE_TESTING");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zval *constant = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC);

    REQUIRE(constant);
    REQUIRE(Z_TYPE_P(constant) == IS_LONG);
    REQUIRE(Z_LVAL_P(constant) == 42);

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup constant [object, does not exist]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn   = ZAI_STRL_VIEW("Stub");
    zai_string_view name = ZAI_STRL_VIEW("DD_TRACE_TESTING_DOES_NOT_EXIST");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC));

    ZAI_VALUE_DTOR(object);
})

TEST_USE("symbol/lookup constant [incorrect use]", {
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_STATIC, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CONSTANT, ZAI_SYMBOL_SCOPE_FRAME, NULL, NULL ZAI_TSRMLS_CC));
})
