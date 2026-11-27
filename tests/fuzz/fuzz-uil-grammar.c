/*
 * Fuzz target: the UIL source grammar, lexical analyzer, semantic validator,
 * and UID emitter (clients/uil/Uil*.c + tools/wml/Uil.y, compiled unchanged
 * with -DCALLABLE).
 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <uil/UilDef.h>

static char g_inc_dir[512];
static char g_helper_path[560];
static char g_uil_path[560];
static char g_uid_path[560];
static char *g_inc_dirs[1];
static int g_inited;
static unsigned int g_msg_count;

static void cleanup_paths(void)
{
    if (g_uil_path[0])
        (void) unlink(g_uil_path);
    if (g_uid_path[0])
        (void) unlink(g_uid_path);
    if (g_helper_path[0])
        (void) unlink(g_helper_path);
    if (g_inc_dir[0])
        (void) rmdir(g_inc_dir);
}

static Uil_continue_type
fuzz_msg_cb(char *msg_data, int msg_num, int severity,
            const char *msg_text, const char *src_text,
            const char *ptr_text, const char *loc_text,
            int message_count[])
{
    (void) msg_data;
    (void) msg_num;
    (void) severity;
    (void) ptr_text;
    (void) loc_text;
    (void) message_count;
    if (msg_text)
        (void) strlen(msg_text);
    if (src_text)
        (void) strlen(src_text);
    if (++g_msg_count > 64)
        return Uil_k_terminate;
    return Uil_k_continue;
}

static Uil_continue_type
fuzz_status_cb(char *status_data, int pct_complete, int lines_processed,
               const char *current_file, int message_count[])
{
    (void) status_data;
    (void) pct_complete;
    (void) lines_processed;
    (void) message_count;
    if (current_file)
        (void) strlen(current_file);
    return (g_msg_count > 64) ? Uil_k_terminate : Uil_k_continue;
}

static void ensure_paths(void)
{
    if (g_inited)
        return;
    setlocale(LC_ALL, "C");
    const char *td = getenv("TMPDIR");
    if (!td || !td[0])
        td = "/var/tmp";
    snprintf(g_inc_dir, sizeof(g_inc_dir), "%s/fuzz-uil-inc-%ld", td, (long) getpid());
    (void) mkdir(g_inc_dir, 0700);
    snprintf(g_helper_path, sizeof(g_helper_path), "%s/helper.uil", g_inc_dir);
    FILE *hf = fopen(g_helper_path, "wb");
    if (hf) {
        const char *hdr = "value k_helper_margin : 8;\n";
        (void) fwrite(hdr, 1, strlen(hdr), hf);
        fclose(hf);
    }
    snprintf(g_uil_path, sizeof(g_uil_path), "%s/input-%ld.uil", td, (long) getpid());
    snprintf(g_uid_path, sizeof(g_uid_path), "%s/output-%ld.uid", td, (long) getpid());
    g_inc_dirs[0] = g_inc_dir;
    atexit(cleanup_paths);
    g_inited = 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size > 8192)
        return 0;
    ensure_paths();

    /* Reject NUL-heavy or raw path-traversal include attempts to keep runs inside g_inc_dir */
    FILE *fp = fopen(g_uil_path, "wb");
    if (!fp)
        return 0;
    if (size > 0)
        (void) fwrite(data, 1, size, fp);
    fclose(fp);

    g_msg_count = 0;
    Uil_command_type cmd;
    Uil_compile_desc_type desc;
    memset(&cmd, 0, sizeof(cmd));
    memset(&desc, 0, sizeof(desc));

    cmd.source_file = g_uil_path;
    cmd.resource_file = g_uid_path;
    cmd.resource_file_flag = 1;
    cmd.include_dir_count = 1;
    cmd.include_dir = g_inc_dirs;
    cmd.report_info_msg_flag = 1;
    cmd.report_warn_msg_flag = 1;
    cmd.parse_tree_flag = 0;
    cmd.issue_summary = 0;
    cmd.status_update_delay = 32;

    (void) Uil(&cmd, &desc, fuzz_msg_cb, NULL, fuzz_status_cb, NULL);

    unlink(g_uil_path);
    unlink(g_uid_path);
    return 0;
}
