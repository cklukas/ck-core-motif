#!/usr/bin/env python3
"""Run the bounded MWM accelerator formatter against expansion and bad input."""

import os
import subprocess
import tempfile
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nAppendAccelText")
end = source.index("} /* END OF FUNCTION ProcessAccelText */", start)
formatter = source[start : end + len("} /* END OF FUNCTION ProcessAccelText */")]

preamble = r'''
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef int Boolean;
typedef unsigned int Cardinal;
#define True 1
#define False 0
typedef struct {
    unsigned char *pchFrom;
    unsigned char *pchTo;
    int lenFrom;
} KeySub;

static unsigned char alt_from[] = "Alt";
static unsigned char alt_to[] = "AlternateName";
static KeySub substitutions[] = {{alt_from, alt_to, 3}};

static void InitKeySubs(KeySub **substitutions_out, int *count_out)
{
    *substitutions_out = substitutions;
    *count_out = 1;
}

#define XtRealloc(pointer, amount) realloc((pointer), (amount))
'''

main = r'''
int main(void)
{
    unsigned char valid[] = "Alt<Key>F4";
    unsigned char malformed[] = "Alt<Key";
    unsigned char *output = malloc(2);
    size_t capacity = 2;

    if (output == NULL || !ProcessAccelText(valid, valid + strlen((char *)valid),
                                             &output, &capacity) ||
        strcmp((char *)output, "AlternateName+F4") != 0 || capacity <= 2) {
        free(output);
        return 1;
    }
    free(output);

    output = malloc(2);
    capacity = 2;
    if (output == NULL || ProcessAccelText(malformed,
            malformed + strlen((char *)malformed), &output, &capacity)) {
        free(output);
        return 2;
    }
    free(output);
    puts("PASS bounded MWM accelerator formatter");
    return 0;
}
'''

temporary_root = os.environ.get("TMPDIR")
if not temporary_root:
    raise RuntimeError("TMPDIR must name the task-local temporary directory")

with tempfile.TemporaryDirectory(prefix="mwm-accelerator-", dir=temporary_root) as temporary:
    temporary_path = Path(temporary)
    harness = temporary_path / "accelerator.c"
    binary = temporary_path / "accelerator"
    harness.write_text(preamble + formatter + main, encoding="utf-8")
    subprocess.run(
        [os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
         "-DWSM", str(harness), "-o", str(binary)],
        check=True,
    )
    subprocess.run([str(binary)], check=True)
