#!/usr/bin/env python3
"""Run MWM's quoted menu-label parser on complete and incomplete labels."""

import os
import subprocess
import tempfile
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static void ParseMenuItemName (unsigned char **linePP, MenuItem *menuItem)\n{")
end = source.index("\n}\n#endif /* !defined(WSM)", start) + 2
parser = source[start:end]

preamble = r'''
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int Cardinal;
typedef struct { char *clientCommandName; } MenuItem;

void ScanWhitespace(unsigned char **cursor)
{
    while (**cursor && isspace((unsigned char)**cursor))
        ++*cursor;
}

#define XtMalloc(amount) malloc((amount))
'''

main = r'''
int main(void)
{
    unsigned char valid[] = "\"Visible title\"";
    unsigned char unterminated[] = "\"Hidden title";
    unsigned char *cursor;
    MenuItem item = {0};

    cursor = valid;
    ParseMenuItemName(&cursor, &item);
    if (item.clientCommandName == NULL ||
        strcmp(item.clientCommandName, "Visible title") != 0) {
        free(item.clientCommandName);
        return 1;
    }
    free(item.clientCommandName);
    item.clientCommandName = NULL;

    cursor = unterminated;
    ParseMenuItemName(&cursor, &item);
    if (item.clientCommandName != NULL)
        return 2;
    puts("PASS quoted MWM menu labels require a closing quote");
    return 0;
}
'''

temporary_root = os.environ.get("TMPDIR")
if not temporary_root:
    raise RuntimeError("TMPDIR must name the task-local temporary directory")

with tempfile.TemporaryDirectory(prefix="mwm-menu-name-", dir=temporary_root) as temporary:
    temporary_path = Path(temporary)
    harness = temporary_path / "menu_name.c"
    binary = temporary_path / "menu_name"
    harness.write_text(preamble + parser + main, encoding="utf-8")
    subprocess.run(
        [os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
         "-DNO_MULTIBYTE", str(harness), "-o", str(binary)],
        check=True,
    )
    subprocess.run([str(binary)], check=True)
