extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/lookup/local/static]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        zai_sapi_execute_script("./stubs/lookup/local/static/Stub.php"); \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITH_STUB("symbol/lookup/local/static [scalar]", {
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

    zai_string_view var = ZAI_STRL_VIEW("var");

    zval *local = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_LOCAL, ZAI_SYMBOL_SCOPE_STATIC, method, &var ZAI_TSRMLS_CC);

    REQUIRE(local);
    REQUIRE(Z_TYPE_P(local) == IS_LONG);
    REQUIRE(Z_LVAL_P(local) == 42);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/lookup/local/static [refcounted]", {
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

    zai_string_view var = ZAI_STRL_VIEW("var");

    zval *local = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_LOCAL, ZAI_SYMBOL_SCOPE_STATIC, method, &var ZAI_TSRMLS_CC);

    REQUIRE(local);
    REQUIRE(Z_TYPE_P(local) == IS_OBJECT);

    ZAI_VALUE_DTOR(result);
})

TEST_WITH_STUB("symbol/lookup/local/static [reference]", {
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

    zai_string_view var = ZAI_STRL_VIEW("var");

    zval *local = (zval*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_LOCAL, ZAI_SYMBOL_SCOPE_STATIC, method, &var ZAI_TSRMLS_CC);

    REQUIRE(local);
    /* This may seem counter intuitive, this is how we expect zend (and so zai) to behave though ... */
    REQUIRE(Z_TYPE_P(local) == IS_NULL);

    ZAI_VALUE_DTOR(result);
})
