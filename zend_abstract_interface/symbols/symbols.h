#ifndef ZAI_SYMBOLS_H
#define ZAI_SYMBOLS_H
/**
 * zai_symbol_lookup is a single interface for access to Zend symbols:
 *  - classes (global, namespace)
 *  - functions (global, namespace, class, object)
 *  - constants (global, namespace, class, object)
 *  - properties (class, object)
 *  - locals (function static, frame)
 *
 * for caller convenience (avoiding casts), the following helpers are provided:
 *
 *  - zai_symbol_lookup_class
 *  - zai_symbol_lookup_function
 *  - zai_symbol_lookup_constant
 *  - zai_symbol_lookup_property
 *  - zai_symbol_lookup_local

 * zai_symbol_call is a single interface for invocation
 *
 * zai_symbol_new is a single interface for object construction
 */
#include "php.h"

#include "../zai_string/string.h"
#include "zai_compat.h"

#include <stdbool.h>
#include <stdint.h>

// clang-format off
typedef enum {
    /* The return type is zend_class_entry* */
    ZAI_SYMBOL_TYPE_CLASS,
    /* The return type is zend_function* */
    ZAI_SYMBOL_TYPE_FUNCTION,
    /* The return type is zval* */
    ZAI_SYMBOL_TYPE_CONSTANT,
    /* The return type is zval* */
    ZAI_SYMBOL_TYPE_PROPERTY,
    /* The return type is zval* */
    ZAI_SYMBOL_TYPE_LOCAL,
} zai_symbol_type_t;

typedef enum {
    /* The next parameter is expected to be zend_class_entry* */
    ZAI_SYMBOL_SCOPE_CLASS,
    /* The next parameter is expected to be zval* and Z_TYPE_P is IS_OBJECT */
    ZAI_SYMBOL_SCOPE_OBJECT,
    /* The next parameter is expected to be null */
    ZAI_SYMBOL_SCOPE_GLOBAL,
    /* The next parameter is zai_string_view* */
    ZAI_SYMBOL_SCOPE_NAMESPACE,
    /* The next parameter is zend_execute_data* */
    ZAI_SYMBOL_SCOPE_FRAME,
    /* The next symbol is zend_function* with type == ZEND_USER_FUNCTION */
    ZAI_SYMBOL_SCOPE_STATIC,
} zai_symbol_scope_t;

/* LOOKUP */
void* zai_symbol_lookup(
        zai_symbol_type_t symbol_type,
        zai_symbol_scope_t scope_type, void *scope,
        zai_string_view *name ZAI_TSRMLS_DC);

static inline zend_class_entry* zai_symbol_lookup_class(
    zai_symbol_scope_t scope_type, void *scope,
    zai_string_view *name ZAI_TSRMLS_DC) {
    return (zend_class_entry*)
        zai_symbol_lookup(
            ZAI_SYMBOL_TYPE_CLASS,
            scope_type, scope, name ZAI_TSRMLS_CC);
}

static inline zend_function* zai_symbol_lookup_function(
    zai_symbol_scope_t scope_type, void *scope,
    zai_string_view *name ZAI_TSRMLS_DC) {
    return (zend_function*)
        zai_symbol_lookup(
            ZAI_SYMBOL_TYPE_FUNCTION,
            scope_type, scope, name ZAI_TSRMLS_CC);
}

static inline zval* zai_symbol_lookup_constant(
    zai_symbol_scope_t scope_type, void *scope,
    zai_string_view *name ZAI_TSRMLS_DC) {
    return (zval*)
        zai_symbol_lookup(
            ZAI_SYMBOL_TYPE_CONSTANT,
            scope_type, scope, name ZAI_TSRMLS_CC);
}

static inline zval* zai_symbol_lookup_property(
    zai_symbol_scope_t scope_type, void *scope,
    zai_string_view *name ZAI_TSRMLS_DC) {
    return (zval*)
        zai_symbol_lookup(
            ZAI_SYMBOL_TYPE_PROPERTY,
            scope_type, scope, name ZAI_TSRMLS_CC);
}

static inline zval* zai_symbol_lookup_local(
    zai_symbol_scope_t scope_type, void *scope,
    zai_string_view *name ZAI_TSRMLS_DC) {
    return (zval*)
        zai_symbol_lookup(
            ZAI_SYMBOL_TYPE_LOCAL,
            scope_type, scope, name ZAI_TSRMLS_CC);
}

/* CALL */
typedef enum {
    /* The function parameter is zend_function* */
    ZAI_SYMBOL_FUNCTION_KNOWN,
    /* The function parameter is zai_string_view* */
    ZAI_SYMBOL_FUNCTION_NAMED,
} zai_symbol_function_t;

bool zai_symbol_call(
    zai_symbol_scope_t scope_type, void *scope,
    zai_symbol_function_t function_type, void *function,
    zval **rv ZAI_TSRMLS_DC,
    uint32_t argc, ...);

bool zai_symbol_new(zval *zv, zend_class_entry *ce ZAI_TSRMLS_DC, uint32_t argc, ...);
// clang-format on
#endif  // ZAI_SYMBOLS_H
