extern "C" {
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITHOUT_STUB(name, code) TEST_CASE(name, "[symbol/lookup/class]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/lookup/class]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        zai_sapi_execute_script("./stubs/lookup/class/Stub.php"); \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_USE(name, code) TEST_CASE(name, "[symbol/lookup/class][.]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITHOUT_STUB("symbol/lookup class [global, exists]", {
    zai_string_view lower = ZAI_STRL_VIEW("stdclass");
    zai_string_view mixed = ZAI_STRL_VIEW("stdClass");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &lower ZAI_TSRMLS_CC));
    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup class [global, does not exist]", {
    zai_string_view lower = ZAI_STRL_VIEW("nosuchclass");
    zai_string_view mixed = ZAI_STRL_VIEW("NoSuchClass");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &lower ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup class [empty ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("");
    zai_string_view lower = ZAI_STRL_VIEW("stdclass");
    zai_string_view mixed = ZAI_STRL_VIEW("stdClass");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &lower ZAI_TSRMLS_CC));
    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup class [root ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\");
    zai_string_view lower = ZAI_STRL_VIEW("stdclass");
    zai_string_view mixed = ZAI_STRL_VIEW("stdClass");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &lower ZAI_TSRMLS_CC));
    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup class [root ns fqcn, exists]", {
    zai_string_view name = ZAI_STRL_VIEW("\\stdClass");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup class [root ns, does not exist]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\");
    zai_string_view lower = ZAI_STRL_VIEW("nosuchclass");
    zai_string_view mixed = ZAI_STRL_VIEW("NoSuchClass");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &lower ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup class [ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view name = ZAI_STRL_VIEW("Stub");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup class [ns fqcn, exists]", {
    zai_string_view name = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
})

TEST_USE("symbol/lookup class [incorrect use]", {
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_CLASS, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_OBJECT, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_FRAME, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_STATIC, NULL, NULL ZAI_TSRMLS_CC));
})
