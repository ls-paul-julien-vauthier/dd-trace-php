extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/lookup/property]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        zai_sapi_execute_script("./stubs/lookup/property/Stub.php"); \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_USE(name, code) TEST_CASE(name, "[symbol/lookup/property][.]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITH_STUB("symbol/lookup property [public static]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zai_string_view name = ZAI_STRL_VIEW("publicStatic");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);
})

TEST_WITH_STUB("symbol/lookup property [protected static]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zai_string_view name = ZAI_STRL_VIEW("protectedStatic");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);
})

TEST_WITH_STUB("symbol/lookup property [private static]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zai_string_view name = ZAI_STRL_VIEW("privateStatic");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);
})

TEST_WITH_STUB("symbol/lookup property [static access instance property]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zai_string_view name = ZAI_STRL_VIEW("publicProperty");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup property [undeclared static]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zai_string_view name = ZAI_STRL_VIEW("undeclaredStaticProperty");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_CLASS, ce, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup property [public]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view name = ZAI_STRL_VIEW("publicProperty");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup property [protected]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view name = ZAI_STRL_VIEW("protectedProperty");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup property [private]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view name = ZAI_STRL_VIEW("privateProperty");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup property [dynamic]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view name = ZAI_STRL_VIEW("dynamicProperty");

    zval *property = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC);

    REQUIRE(property);
    REQUIRE(Z_TYPE_P(property) == IS_LONG);
    REQUIRE(Z_LVAL_P(property) == 42);

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup property [undeclared]", {
    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(ce);

    if (!ce) {
        return;
    }

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view name = ZAI_STRL_VIEW("undeclaredProperty");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_OBJECT, object, &name ZAI_TSRMLS_CC));

    ZAI_VALUE_DTOR(object);
})

TEST_USE("symbol/lookup property [incorrect use]", {
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_NAMESPACE, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_STATIC, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_PROPERTY, ZAI_SYMBOL_SCOPE_FRAME, NULL, NULL ZAI_TSRMLS_CC));
})
