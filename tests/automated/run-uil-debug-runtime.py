#!/usr/bin/env python3
"""Execute the production UIL debug formatter with sanitizer instrumentation."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import resource
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source', type=Path,
                    default=Path(__file__).resolve().parents[2] / 'clients/uil/UilLstLst.c')
parser.add_argument('--out', required=True, type=Path)
args = parser.parse_args()
resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
args.out.mkdir(parents=True, exist_ok=False)
tmp = args.out / 'tmp'
tmp.mkdir()
env = dict(os.environ, TMPDIR=str(tmp))
source = args.source.read_text()
# These adjacent production definitions have no other toolkit dependencies.
start = source.index('static void\nlst_debug_append(')
functions = source[start:]
harness = r'''
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned int Cardinal;
#define FALSE 0
static int lst_v_listing_open = 1;
static char lst_debug_buffer[132];
static int lst_debug_cur_pos;
static char captured[40000];
static size_t used, lines;
static int fail_allocation;
static void lst_debug_flush(void);
static void *_get_memory(Cardinal n) { return fail_allocation ? NULL : malloc(n); }
static void _free_memory(void *p) { free(p); }
static void lst_output_line(char *s, int flag) {
    size_t n = strlen(s);
    (void)flag;
    assert(n <= 131 && used + n < sizeof(captured));
    memcpy(captured + used, s, n);
    used += n; captured[used] = 0; ++lines;
}
'''
checks = r'''
int main(void) {
    char text[20001];
    memset(text, 'Q', sizeof(text)-1); text[20000] = 0;
    lst_debug_output("%s", text);
    lst_debug_flush();
    assert(used == 20000 && memcmp(captured, text, 20000) == 0);
    assert(lines == 153 && lst_debug_cur_pos == 0);
    used = lines = 0;
    lst_debug_output("%s", "alpha");
    assert(lines == 0);
    lst_debug_output("%s", "beta\n\ngamma");
    assert(lines == 2 && used == 9);
    lst_debug_flush();
    assert(lines == 3 && used == 14);
    assert(memcmp(captured, "alphabetagamma", 14) == 0);
    used = lines = 0;
    memset(text, 'R', 131); text[131] = 0;
    lst_debug_output("%s", text);
    lst_debug_output("%s", "Z\n");
    assert(lines == 2 && used == 132 && captured[131] == 'Z');
    /* Deterministic fragmented stream: compare emitted bytes against a
     * separate newline-stripping oracle across many flush boundaries. */
    {
        unsigned seed = 1729;
        char stream[12001], expected[12001];
        size_t expected_size = 0, offset = 0;
        used = lines = 0;
        for (size_t i = 0; i < 12000; ++i) {
            seed = seed * 1664525U + 1013904223U;
            stream[i] = seed % 47 == 0 ? '\n' : (char)('a' + seed % 26);
            if (stream[i] != '\n') expected[expected_size++] = stream[i];
        }
        stream[12000] = 0;
        while (offset < 12000) {
            size_t count = 1 + seed % 300;
            if (count > 12000 - offset) count = 12000 - offset;
            lst_debug_output("%.*s", (int)count, stream + offset);
            offset += count;
            seed = seed * 1664525U + 1013904223U;
        }
        lst_debug_flush();
        assert(used == expected_size);
        assert(memcmp(captured, expected, used) == 0);
    }
    used = lines = 0;
    fail_allocation = 1;
    lst_debug_output("%s", "ignored");
    assert(used == 0 && lst_debug_cur_pos == 0);
    puts("PASS production UIL formatter: long output, fragments, boundaries, allocation failure");
}
'''
program = args.out / 'uil-debug-runtime.c'
program.write_text(harness + functions + checks)
binary = args.out / 'uil-debug-runtime'
receipt = {'source': str(args.source.resolve()),
           'source_sha256': hashlib.sha256(args.source.read_bytes()).hexdigest(),
           'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'duration_baseline': 'not established', 'steps': [], 'success': False}
began = time.monotonic()
try:
    for command in [
        ['clang', '-std=c99', '-Wall', '-Wextra', '-Werror',
         '-fsanitize=address,undefined', str(program), '-o', str(binary)],
        [str(binary)]]:
        result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=30)
        receipt['steps'].append({'command': command, 'exit': result.returncode,
                                 'output': result.stdout + result.stderr})
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
    receipt['success'] = True
finally:
    receipt['seconds'] = time.monotonic() - began
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
print('PASS production UIL formatter under ASan/UBSan')
