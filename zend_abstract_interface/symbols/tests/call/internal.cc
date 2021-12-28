extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITHOUT_STUB(name, code) TEST_CASE(name, "[symbol/call/internal]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITHOUT_STUB("symbol/call/internal [global]", {
    zval *param;

    ZAI_VALUE_MAKE(param);
    ZAI_VALUE_STRINGL(param, "string", sizeof("string")-1);

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view fn = ZAI_STRL_VIEW("\\strlen");

    zai_symbol_call(ZAI_SYMBOL_SCOPE_GLOBAL, NULL, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 1, &param);

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 6);

    ZAI_VALUE_DTOR(param);
    ZAI_VALUE_DTOR(result);
})

TEST_WITHOUT_STUB("symbol/call/internal [root ns]", {
    zval *param;

    ZAI_VALUE_MAKE(param);
    ZAI_VALUE_STRINGL(param, "string", sizeof("string")-1);

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("\\");
    zai_string_view fn = ZAI_STRL_VIEW("strlen");

    zai_symbol_call(ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 1, &param);

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 6);

    ZAI_VALUE_DTOR(param);
    ZAI_VALUE_DTOR(result);
})

TEST_WITHOUT_STUB("symbol/call/internal [empty ns]", {
    zval *param;

    ZAI_VALUE_MAKE(param);
    ZAI_VALUE_STRINGL(param, "string", sizeof("string")-1);

    zval *result;
    ZAI_VALUE_INIT(result);

    zai_string_view ns = ZAI_STRL_VIEW("");
    zai_string_view fn = ZAI_STRL_VIEW("strlen");

    zai_symbol_call(ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, ZAI_SYMBOL_FUNCTION_NAMED, &fn, &result ZAI_TSRMLS_CC, 1, &param);

    REQUIRE(Z_TYPE_P(result) == IS_LONG);
    REQUIRE(Z_LVAL_P(result) == 6);

    ZAI_VALUE_DTOR(param);
    ZAI_VALUE_DTOR(result);
})
