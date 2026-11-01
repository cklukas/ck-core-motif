/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <uil/Uil.h>
#include <stdio.h>
#include <string.h>

typedef struct { unsigned messages, progress; } Counts;

static Uil_continue_type message(char *data, int number, int severity,
    const char *text, const char *source, const char *pointer,
    const char *location, int *summary)
{
    Counts *counts = (Counts *)data;
    (void)number; (void)severity; (void)source;
    (void)pointer; (void)location;
    if (!text || !summary) return Uil_k_terminate;
    ++counts->messages;
    return Uil_k_continue;
}

static Uil_continue_type progress(char *data, int percent, int lines,
    const char *file, int *summary)
{
    Counts *counts = (Counts *)data;
    (void)lines; (void)file;
    if (percent < 0 || percent > 100 || !summary) return Uil_k_terminate;
    ++counts->progress;
    return Uil_k_continue;
}

int main(int argc, char **argv)
{
    Uil_command_type command = {0};
    Uil_compile_desc_type result = {0};
    Counts counts = {0};
    Uil_status_type status;
    if (argc != 2) return 1;
    command.source_file = argv[1];
    command.report_info_msg_flag = 1;
    command.report_warn_msg_flag = 1;
    command.issue_summary = 1;
    command.status_update_delay = 0;
    status = Uil(&command, &result, message, (char *)&counts,
                 progress, (char *)&counts);
    if (status > Uil_k_warning_status || counts.progress == 0) {
        fprintf(stderr, "UIL valid-input status=%u progress=%u messages=%u\n",
                status, counts.progress, counts.messages);
        return 1;
    }
    command.source_file = "";
    counts.messages = 0;
    status = Uil(&command, &result, message, (char *)&counts,
                 progress, (char *)&counts);
    if (status < Uil_k_error_status || counts.messages == 0) return 1;
    puts("PASS typed UIL progress and diagnostic callbacks");
    return 0;
}
