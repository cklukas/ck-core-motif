#!/usr/bin/env python3
"""Exercise the production UIL fixed-field filename copy under sanitizers."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out', type=Path, required=True)
args = parser.parse_args()
if not args.out.is_absolute():
    parser.error('--out must be an absolute fresh directory')
args.out.mkdir(parents=True, exist_ok=False)
(args.out / 'tmp').mkdir()
env = dict(os.environ, TMPDIR=str(args.out / 'tmp'))
source = Path(__file__).resolve().parents[2] / 'clients/uil/UilSrcSrc.c'
content = source.read_text()
start = content.index('static void\nsrc_copy_file_name(')
begin = content.index('{', start)
end, depth = begin + 1, 1
while depth:
    depth += (content[end] == '{') - (content[end] == '}')
    end += 1
body = content[start:end] + '\n'
program = args.out / 'uil-source-path-copy.c'
program.write_text(r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define XmConst const
''' + body + r'''
int main(void) {
    struct { char field[256]; char guard[8]; } bounded;
    char long_name[2049];
    memset(long_name, 'p', sizeof(long_name) - 1);
    long_name[sizeof(long_name) - 1] = 0;
    memset(&bounded, 0, sizeof(bounded));
    memset(bounded.guard, 'G', sizeof(bounded.guard));
    src_copy_file_name(bounded.field, sizeof(bounded.field), long_name);
    assert(strlen(bounded.field) == sizeof(bounded.field) - 1);
    assert(!memcmp(bounded.field, long_name, sizeof(bounded.field) - 1));
    for (unsigned int i = 0; i < sizeof(bounded.guard); ++i)
        assert(bounded.guard[i] == 'G');
    src_copy_file_name(bounded.field, sizeof(bounded.field), "relative.uil");
    assert(!strcmp(bounded.field, "relative.uil"));
    src_copy_file_name(bounded.field, sizeof(bounded.field), NULL);
    assert(bounded.field[0] == 0);
    src_copy_file_name(NULL, 0, long_name);
    src_copy_file_name(bounded.field, 0, long_name);
    puts("PASS production UIL filename copy: long, ordinary, null, zero-sized destination");
}
''')
binary = args.out / 'uil-source-path-copy'
receipt = {'source': str(source),
           'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
           'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'duration_baseline': 'not established', 'steps': [], 'success': False}
began = time.monotonic()
try:
    for command in [
        ['clang', '-std=c99', '-Wall', '-Wextra', '-Werror',
         '-fsanitize=address,undefined', str(program), '-o', str(binary)],
        [str(binary)],
    ]:
        result = subprocess.run(command, env=env, capture_output=True, text=True,
                                timeout=30)
        receipt['steps'].append({'command': command, 'exit': result.returncode,
                                 'output': result.stdout + result.stderr})
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
    receipt['success'] = True
finally:
    receipt['seconds'] = time.monotonic() - began
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
print('PASS UIL source-path copy regression under ASan/UBSan')
