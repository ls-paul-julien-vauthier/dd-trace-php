#ifndef DDTRACE_H
#define DDTRACE_H
#include <dogstatsd_client/client.h>
#include <stdbool.h>
#include <stdint.h>

#include "ext/version.h"
#include "random.h"

extern zend_module_entry ddtrace_module_entry;
extern zend_class_entry *ddtrace_ce_span_data;
extern zend_class_entry *ddtrace_ce_fatal_error;

typedef struct ddtrace_span_ids_t ddtrace_span_ids_t;
typedef struct ddtrace_span_fci ddtrace_span_fci;
typedef struct ddtrace_span_t ddtrace_span_t;

zval *ddtrace_spandata_property_name(ddtrace_span_t *span);
zval **ddtrace_spandata_property_name_write(ddtrace_span_t *span);
zval *ddtrace_spandata_property_resource(ddtrace_span_t *span);
zval **ddtrace_spandata_property_resource_write(ddtrace_span_t *span);
zval *ddtrace_spandata_property_service(ddtrace_span_t *span);
zval **ddtrace_spandata_property_service_write(ddtrace_span_t *span);
zval *ddtrace_spandata_property_type(ddtrace_span_t *span);
zval **ddtrace_spandata_property_type_write(ddtrace_span_t *span);
zval *ddtrace_spandata_property_meta(ddtrace_span_t *span);
zval *ddtrace_spandata_property_metrics(ddtrace_span_t *span);
zval *ddtrace_spandata_property_exception(ddtrace_span_t *span);
zval **ddtrace_spandata_property_exception_write(ddtrace_span_t *span);
zval *ddtrace_spandata_property_parent(ddtrace_span_t *span);
zval **ddtrace_spandata_property_parent_write(ddtrace_span_t *span);

bool ddtrace_tracer_is_limited(TSRMLS_D);
// prepare the tracer state to start handling a new trace
void dd_prepare_for_new_trace(TSRMLS_D);
void ddtrace_disable_tracing_in_current_request(void);
bool ddtrace_alter_dd_trace_disabled_config(zval *old_value, zval *new_value);

typedef struct {
    int type;
    zval *message;
} ddtrace_error_data;

// clang-format off
ZEND_BEGIN_MODULE_GLOBALS(ddtrace)
    char *auto_prepend_file;
    uint8_t disable; // 0 = enabled, 1 = disabled via INI, 2 = disabled, but MINIT was fully executed
    zend_bool request_init_hook_loaded;

    uint32_t traces_group_id;
    HashTable *class_lookup;
    HashTable *function_lookup;
    zval additional_trace_meta; // IS_ARRAY
    HashTable additional_global_tags;
    HashTable root_span_tags_preset;
    HashTable propagated_root_span_tags;
    zend_bool log_backtrace;
    zend_bool backtrace_handler_already_run;
    ddtrace_error_data active_error;
    dogstatsd_client dogstatsd_client;
    char *dogstatsd_host;
    char *dogstatsd_port;
    char *dogstatsd_buffer;

    // Distributed tracing & curl
    HashTable *curl_headers;
    zend_bool curl_back_up_headers;
    int le_curl;  // ext/curl's list entry resource type
    HashTable *curl_multi_handles;
    HashTable *curl_multi_handles_cache;
    int curl_multi_handles_cache_id;
    HashTable *curl_bug_71523_copied_ch;

    /* These ones are used for measuring the call stack depth so that we can
     * emit a warning prior to encountering a stack overflow.
     *
     * A 16-bit call depth would allow us to count to 65,535, which is way more
     * than necessary. An 8-bit depth would be inadequate (255).
     */
    bool has_warned_call_depth;
    uint16_t call_depth;

    uint64_t trace_id;
    long default_priority_sampling;
    long propagated_priority_sampling;
    ddtrace_span_ids_t *span_ids_top;
    ddtrace_span_fci *open_spans_top;
    ddtrace_span_fci *closed_spans_top;
    ddtrace_span_fci *root_span;
    uint32_t open_spans_count;
    uint32_t closed_spans_count;
    int64_t compile_time_microseconds;
    uint64_t distributed_parent_trace_id;
    char *dd_origin;

    char *cgroup_file;
ZEND_END_MODULE_GLOBALS(ddtrace)
// clang-format on

#ifdef ZTS
#define DDTRACE_G(v) TSRMG(ddtrace_globals_id, zend_ddtrace_globals *, v)
#else
#define DDTRACE_G(v) (ddtrace_globals.v)
#endif

#define PHP_DDTRACE_EXTNAME "ddtrace"
#ifndef PHP_DDTRACE_VERSION
#define PHP_DDTRACE_VERSION "0.0.0-unknown"
#endif

#define DDTRACE_CALLBACK_NAME "dd_trace_callback"

/* The clang formatter does not handle the ZEND macros these mirror, due to the
 * missing comma in the usage site. It was making PRs unreviewable, so this
 * defines these macros without the comma in the definition site, so that it
 * exists at the usage site.
 */
#define DDTRACE_ARG_INFO_SIZE(arg_info) ((zend_uint)(sizeof(arg_info) / sizeof(struct _zend_arg_info) - 1))

#define DDTRACE_FENTRY(zend_name, name, arg_info, flags) \
    { #zend_name, name, arg_info, DDTRACE_ARG_INFO_SIZE(arg_info), flags }
#define DDTRACE_RAW_FENTRY(zend_name, name, arg_info, flags) \
    { zend_name, name, arg_info, DDTRACE_ARG_INFO_SIZE(arg_info), flags }

#define DDTRACE_FE(name, arg_info) DDTRACE_FENTRY(name, zif_##name, arg_info, 0)
#define DDTRACE_NS_FE(name, arg_info) DDTRACE_RAW_FENTRY("DDTrace\\" #name, zif_##name, arg_info, 0)
#define DDTRACE_SUB_NS_FE(ns, name, arg_info) DDTRACE_RAW_FENTRY("DDTrace\\" ns #name, zif_##name, arg_info, 0)
#define DDTRACE_FALIAS(name, alias, arg_info) DDTRACE_RAW_FENTRY(#name, zif_##alias, arg_info, 0)
#define DDTRACE_FE_END ZEND_FE_END

/* Currently used on PHP 5. After a zend_execute_ex has called the previous hook
 * the execute_data cannot be trusted for some things, notably function_state.
 * So we use this struct to back up the data.
 */
struct ddtrace_execute_data {
    zval *This;
    zend_class_entry *scope;
    zend_function *fbc;
    const zend_op *opline;
    void **arguments;
    zval *retval;
    bool free_retval;
};
typedef struct ddtrace_execute_data ddtrace_execute_data;

#endif  // DDTRACE_H
