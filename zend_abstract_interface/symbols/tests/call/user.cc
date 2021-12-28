extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/call/user]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        REQUIRE(zai_sapi_execute_script("./stubs/call/user/Stub.php")); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITH_STUB("symbol/call/user [global function]", {
    zval *param;

    ZAI_VALUE_MAKE(param);
    ZAI_VALUE_STRINGL(param, "string", sizeof("string")-1);

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view fn = ZAI_STRL_VIEW("\\stub");

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_GLOBAL, NULL, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 1, &param));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 6);

    ZAI_VALUE_DTOR(param);
    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/call/user [ns function]", {
    zval *param;

    ZAI_VALUE_MAKE(param);
    ZAI_VALUE_STRINGL(param, "string", sizeof("string")-1);

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view fn = ZAI_STRL_VIEW("stub");

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 1, &param));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 6);

    ZAI_VALUE_DTOR(param);
    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/call/user [static public]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");
    zai_string_view fn = ZAI_STRL_VIEW("staticPublicFunction");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 42);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/call/user [static protected]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");
    zai_string_view fn = ZAI_STRL_VIEW("staticProtectedFunction");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 42);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/call/user [static private]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");
    zai_string_view fn = ZAI_STRL_VIEW("staticPrivateFunction");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 42);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/call/user [instance public]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view fn = ZAI_STRL_VIEW("publicFunction");

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_OBJECT, object, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 42);

    ZAI_VALUE_DTOR(result);
    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/call/user [instance protected]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view fn = ZAI_STRL_VIEW("protectedFunction");

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_OBJECT, object, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 42);

    ZAI_VALUE_DTOR(result);
    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/call/user [instance private]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("Stub");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view fn = ZAI_STRL_VIEW("privateFunction");

    REQUIRE(zai_symbol_call(ZAI_SYMBOL_SCOPE_OBJECT, object, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 42);

    ZAI_VALUE_DTOR(result);
    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/call/user [no magic]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zval *object;
    ZAI_VALUE_MAKE(object);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("NoMagicCall");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    zai_string_view fn = ZAI_STRL_VIEW("nonExistent");

    REQUIRE(!zai_symbol_call(ZAI_SYMBOL_SCOPE_OBJECT, object, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/call/user [no abstract]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("NoAbstractCall");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_string_view fn = ZAI_STRL_VIEW("abstractFunction");

    REQUIRE(!zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));
})

TEST_WITH_STUB("symbol/call/user [static mismatch]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("NoStaticMismatch");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_string_view fn = ZAI_STRL_VIEW("nonStaticFunction");

    REQUIRE(!zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));
})

TEST_WITH_STUB("symbol/call/user [exception]", {
    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view cn = ZAI_STRL_VIEW("NoExceptionLeakage");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &cn ZAI_TSRMLS_CC);

    zai_string_view fn = ZAI_STRL_VIEW("throwsException");

    REQUIRE(!zai_symbol_call(ZAI_SYMBOL_SCOPE_CLASS, ce, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 0));
})
