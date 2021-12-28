extern "C" {
#include "value/value.h"
#include "symbols/symbols.h"
#include "zai_sapi/zai_sapi.h"

#include "zai_compat.h"
}

#include <catch2/catch.hpp>
#include <cstdlib>
#include <cstring>

#define TEST_WITHOUT_STUB(name, code) TEST_CASE(name, "[symbol/lookup/function]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_USE(name, code) TEST_CASE(name, "[symbol/lookup/function][.]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

#define TEST_WITH_STUB(name, code) TEST_CASE(name, "[symbol/lookup/function]") { \
        REQUIRE(zai_sapi_sinit()); \
        REQUIRE(zai_sapi_minit()); \
        REQUIRE(zai_sapi_rinit()); \
        ZAI_SAPI_TSRMLS_FETCH(); \
        ZAI_SAPI_ABORT_ON_BAILOUT_OPEN() \
        zai_sapi_execute_script("./stubs/lookup/function/Stub.php"); \
        { code } \
        ZAI_SAPI_ABORT_ON_BAILOUT_CLOSE() \
        zai_sapi_spindown(); \
    }

TEST_WITHOUT_STUB("symbol/lookup function [global, exists]", {
    zai_string_view lower = ZAI_STRL_VIEW("strlen");
    zai_string_view mixed = ZAI_STRL_VIEW("strLen");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &lower ZAI_TSRMLS_CC));
    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup function [global, does not exist]", {
    zai_string_view lower = ZAI_STRL_VIEW("nosuchfunction");
    zai_string_view mixed = ZAI_STRL_VIEW("NoSuchFunction");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &lower ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup function [root ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\");
    zai_string_view lower = ZAI_STRL_VIEW("strlen");
    zai_string_view mixed = ZAI_STRL_VIEW("strLen");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &lower ZAI_TSRMLS_CC));
    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup function [root ns fqcn, exists]", {
    zai_string_view name = ZAI_STRL_VIEW("\\strlen");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
})

TEST_WITHOUT_STUB("symbol/lookup function [root ns, does not exist]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\");
    zai_string_view lower = ZAI_STRL_VIEW("nosuchfunction");
    zai_string_view mixed = ZAI_STRL_VIEW("NoSuchFunction");

    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &lower ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &mixed ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup function [ns, exists]", {
    zai_string_view ns   = ZAI_STRL_VIEW("\\DDTraceTesting");
    zai_string_view name = ZAI_STRL_VIEW("StubFunction");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_NAMESPACE, &ns, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup function [ns fqcn, exists]", {
    zai_string_view name = ZAI_STRL_VIEW("\\DDTraceTesting\\StubFunction");

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &name ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup function [class method exists, public]", {
    zai_string_view scope = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");
    zai_string_view method = ZAI_STRL_VIEW("existsPublic");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &scope ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &method ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup function [class method exists, protected]", {
    zai_string_view scope = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");
    zai_string_view method = ZAI_STRL_VIEW("existsProtected");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &scope ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &method ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup function [class method exists, private]", {
    zai_string_view scope = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");
    zai_string_view method = ZAI_STRL_VIEW("existsPrivate");

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &scope ZAI_TSRMLS_CC);

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_CLASS, ce, &method ZAI_TSRMLS_CC));
})

TEST_WITH_STUB("symbol/lookup function [object method exists, public]", {
    zai_string_view scope = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");
    zai_string_view method = ZAI_STRL_VIEW("existsPublic");

    zval *object;
    ZAI_VALUE_MAKE(object);

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &scope ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_OBJECT, object, &method ZAI_TSRMLS_CC));

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup function [object method exists, protected]", {
    zai_string_view scope = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");
    zai_string_view method = ZAI_STRL_VIEW("existsProtected");

    zval *object;
    ZAI_VALUE_MAKE(object);

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &scope ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_OBJECT, object, &method ZAI_TSRMLS_CC));

    ZAI_VALUE_DTOR(object);
})

TEST_WITH_STUB("symbol/lookup function [object method exists, private]", {
    zai_string_view scope = ZAI_STRL_VIEW("\\DDTraceTesting\\Stub");
    zai_string_view method = ZAI_STRL_VIEW("existsPrivate");

    zval *object;
    ZAI_VALUE_MAKE(object);

    zend_class_entry *ce = (zend_class_entry*) zai_symbol_lookup(ZAI_SYMBOL_TYPE_CLASS, ZAI_SYMBOL_SCOPE_GLOBAL, NULL, &scope ZAI_TSRMLS_CC);

    zai_symbol_new(object, ce ZAI_TSRMLS_CC, 0);

    REQUIRE(zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_OBJECT, object, &method ZAI_TSRMLS_CC));

    ZAI_VALUE_DTOR(object);
})

TEST_USE("symbol/lookup function [incorrect use]", {
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_STATIC, NULL, NULL ZAI_TSRMLS_CC));
    REQUIRE(!zai_symbol_lookup(ZAI_SYMBOL_TYPE_FUNCTION, ZAI_SYMBOL_SCOPE_FRAME, NULL, NULL ZAI_TSRMLS_CC));
})
