/*
 * Fuzz target: the UIL binary widget meta-language database (.wmd) reader
 * (clients/uil/UilDB.c).
 *
 * run-fuzz.py extracts db_read_table, db_allocate_table, db_incorporate,
 * db_read_ints_and_string, db_read_char_table, db_read_length_and_string,
 * db_read_int_and_shorts, db_open_file, get_root_dir_name and init_wmd_path
 * from clients/uil/UilDB.c into production.h.
 *
 * The input is written to a temporary .wmd file in TMPDIR and loaded through
 * db_incorporate().  Every allocation performed by UilDB.c is tracked in an
 * exact-sized heap block (capped at 256 KB per allocation so crafted headers
 * exercise the NULL-allocation error paths without exhausting memory) and
 * released after each iteration.
 */
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <X11/Intrinsic.h>
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include "UilDBDef.h"

#ifndef externalref
#define externalref extern
#endif
#include "UilSymGl.h"

/* UIL global table definitions populated by db_incorporate() */
unsigned char *constraint_tab;
key_keytable_entry_type *key_table;
int key_k_keyword_max_length;
int key_k_keyword_count;
key_keytable_entry_type *key_table_case_ins;
unsigned char **allowed_argument_table;
unsigned char *argument_type_table;
unsigned char **allowed_child_table;
unsigned char *child_class_table;
char **charset_xmstring_names_table;
unsigned char *charset_writing_direction_table;
unsigned char *charset_parsing_direction_table;
unsigned char *charset_character_size_table;
char **charset_lang_names_table;
unsigned short int *charset_lang_codes_table;
unsigned short int charset_lang_table_max;
unsigned char **allowed_control_table;
UilEnumSetDescDef *enum_set_table;
unsigned short int *argument_enumset_table;
int *enumval_values_table;
int uil_max_object;
char **uil_widget_names;
int uil_max_arg;
char **uil_argument_names;
int uil_max_child;
char **uil_child_names;
int uil_max_reason;
char **uil_reason_names;
int uil_max_enumset;
int uil_max_enumval;
char **uil_enumval_names;
int uil_max_charset;
char **uil_charset_names;
unsigned short int *related_argument_table;
unsigned char **allowed_reason_table;
char **tok_token_name_table;
int tok_num_tokens;
char **uil_widget_funcs;
unsigned short int *uil_gadget_variants;
unsigned short int *uil_urm_nondialog_class;
unsigned short int *uil_urm_subtree_resource;
char **uil_argument_toolkit_names;
char **uil_reason_toolkit_names;
unsigned short int uil_sym_user_defined_object;
unsigned short int uil_sym_default_charset;
unsigned short int uil_sym_isolatin1_charset;
int uil_max_value;
char *uil_datatype_names[1];

#ifndef USER_VAR
#define USER_VAR "USER"
#endif

typedef struct {
    char *ac_database;
} fuzz_cmd_type;

static fuzz_cmd_type Uil_cmd_z_command;

enum {
    d_bad_database = 1,
    d_future_version = 2,
    d_wmd_open = 3,
    d_src_open = 4,
    diag_k_no_source = 0,
    diag_k_no_column = 0,
};

static unsigned int diagnostics_issued;

static void diag_issue_diagnostic(int d_message_number, ...)
{
    (void) d_message_number;
    diagnostics_issued++;
}

static void diag_issue_internal_error(const char *msg)
{
    (void) msg;
    diagnostics_issued++;
}

/* Per-iteration tracked allocator so repeated db_incorporate() calls do not
 * leak tables and oversized headers test NULL-return recovery cleanly. */
enum {
    MAX_TRACKED_ALLOCS = 4096,
    MAX_SINGLE_ALLOC = 256 * 1024,
};

static void *tracked_allocs[MAX_TRACKED_ALLOCS];
static size_t tracked_count;

static char *fuzz_xt_malloc(Cardinal size)
{
    if (size > MAX_SINGLE_ALLOC || tracked_count >= MAX_TRACKED_ALLOCS)
        return NULL;
    void *p = malloc(size ? size : 1);
    if (!p) return NULL;
    tracked_allocs[tracked_count++] = p;
    return (char *) p;
}

static char *fuzz_xt_calloc(Cardinal num, Cardinal size)
{
    if (num != 0 && size > MAX_SINGLE_ALLOC / num)
        return NULL;
    size_t total = (size_t) num * (size_t) size;
    if (total > MAX_SINGLE_ALLOC || tracked_count >= MAX_TRACKED_ALLOCS)
        return NULL;
    void *p = calloc(1, total ? total : 1);
    if (!p) return NULL;
    tracked_allocs[tracked_count++] = p;
    return (char *) p;
}

static void fuzz_xt_free(char *ptr)
{
    if (!ptr) return;
    for (size_t i = 0; i < tracked_count; i++) {
        if (tracked_allocs[i] == ptr) {
            free(ptr);
            tracked_allocs[i] = tracked_allocs[--tracked_count];
            return;
        }
    }
    XtFree(ptr);
}

static String fuzz_xt_new_string(String str)
{
    if (!str) return NULL;
    size_t len = strlen(str);
    if (len >= MAX_SINGLE_ALLOC) return NULL;
    char *copy = fuzz_xt_malloc((Cardinal) (len + 1));
    if (copy) memcpy(copy, str, len + 1);
    return copy;
}

static void reset_uil_tables(void)
{
    for (size_t i = 0; i < tracked_count; i++)
        free(tracked_allocs[i]);
    tracked_count = 0;
    constraint_tab = NULL;
    key_table = NULL;
    key_table_case_ins = NULL;
    allowed_argument_table = NULL;
    argument_type_table = NULL;
    allowed_child_table = NULL;
    child_class_table = NULL;
    charset_xmstring_names_table = NULL;
    charset_writing_direction_table = NULL;
    charset_parsing_direction_table = NULL;
    charset_character_size_table = NULL;
    charset_lang_names_table = NULL;
    charset_lang_codes_table = NULL;
    allowed_control_table = NULL;
    enum_set_table = NULL;
    argument_enumset_table = NULL;
    enumval_values_table = NULL;
    uil_widget_names = NULL;
    uil_argument_names = NULL;
    uil_child_names = NULL;
    uil_reason_names = NULL;
    uil_enumval_names = NULL;
    uil_charset_names = NULL;
    related_argument_table = NULL;
    allowed_reason_table = NULL;
    uil_widget_funcs = NULL;
    uil_gadget_variants = NULL;
    uil_urm_nondialog_class = NULL;
    uil_urm_subtree_resource = NULL;
    uil_argument_toolkit_names = NULL;
    uil_reason_toolkit_names = NULL;
    uil_sym_user_defined_object = 0;
}

#define XtMalloc fuzz_xt_malloc
#define XtCalloc fuzz_xt_calloc
#define XtFree fuzz_xt_free
#undef XtNewString
#define XtNewString fuzz_xt_new_string

void db_incorporate(void);
Boolean db_read_ints_and_string(_db_header_ptr header);
Boolean db_read_char_table(_db_header_ptr header);
Boolean db_read_length_and_string(_db_header_ptr header);
Boolean db_read_int_and_shorts(_db_header_ptr header);
void db_open_file(void);
String get_root_dir_name(void);
String init_wmd_path(String filename);

#include FUZZ_PRODUCTION_H

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    static char path[1024];
    if (!path[0]) {
        const char *tmp = getenv("TMPDIR");
        snprintf(path, sizeof(path), "%s/uil-wmd-input-%d.wmd",
                 tmp ? tmp : ".", (int) getpid());
    }

    FILE *f = fopen(path, "wb");
    if (!f) abort();
    if (size) fwrite(data, 1, size, f);
    fclose(f);

    diagnostics_issued = 0;
    reset_uil_tables();
    Uil_cmd_z_command.ac_database = path;

    db_incorporate();

    reset_uil_tables();
    unlink(path);
    return 0;
}
