#!/usr/bin/env python3
"""Exercise production UIL machine-code input guards under ASan/UBSan."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--out", type=Path, required=True)
args = parser.parse_args()
if not args.out.is_absolute() or args.out.exists():
    parser.error("--out must be a fresh absolute task directory")
args.out.mkdir(parents=True)
(args.out / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(args.out / "tmp"))
source = Path(__file__).resolve().parents[2] / "clients/uil/UilSrcSrc.c"
content = source.read_text(encoding="utf-8")
start = content.index("void\nsrc_append_machine_code")
begin = content.index("{", start)
end, depth = begin + 1, 1
while depth:
    depth += (content[end] == "{") - (content[end] == "}")
    end += 1
body = content[start:end] + "\n"
program = args.out / "uil-machine-code-inputs.c"
program.write_text(r'''
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define XmConst const
typedef unsigned int Cardinal;
typedef struct _src_machine_code_type {
    struct _src_machine_code_type *az_next_machine_code;
    unsigned short w_offset, w_code_len;
    union { long q_longdata[1]; char c_data[1]; } data;
} src_machine_code_type;
typedef struct _src_source_record_type {
    src_machine_code_type *az_machine_code_list;
    unsigned short w_machine_code_cnt;
} src_source_record_type;
static void *_get_memory(Cardinal size) { return calloc(1, size); }
static void _move(void *to, const void *from, size_t size) { memcpy(to, from, size); }
''' + body + r'''
int main(void) {
    src_source_record_type source = {0};
    char bytes[] = {1, 2, 3, 4};
    src_append_machine_code(&source, 4, (int)sizeof(bytes), bytes, "data");
    assert(source.w_machine_code_cnt == 1);
    assert(source.az_machine_code_list != NULL);
    assert(source.az_machine_code_list->w_code_len == sizeof(bytes));
    assert(!memcmp(source.az_machine_code_list->data.c_data, bytes, sizeof(bytes)));
    src_append_machine_code(&source, 0, 1, NULL, "must not append");
    assert(source.w_machine_code_cnt == 1);
    source.w_machine_code_cnt = USHRT_MAX;
    src_append_machine_code(&source, 0, 1, bytes, "must not wrap");
    assert(source.w_machine_code_cnt == USHRT_MAX);
    src_append_machine_code(NULL, 0, 1, bytes, "must not dereference");
    free(source.az_machine_code_list);
    puts("PASS production UIL machine-code input guards");
}
''', encoding="utf-8")
binary = args.out / "uil-machine-code-inputs"
receipt = {
    "source": str(source), "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
    "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "success": False, "steps": []
}
began = time.monotonic()
try:
    for command in [
        ["clang", "-std=c99", "-Wall", "-Wextra", "-Werror", "-fsanitize=address,undefined", str(program), "-o", str(binary)],
        [str(binary)],
    ]:
        result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=30)
        receipt["steps"].append({"command": command, "exit": result.returncode, "output": result.stdout + result.stderr})
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
    receipt["success"] = True
finally:
    receipt["seconds"] = time.monotonic() - began
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.out / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
print("PASS UIL machine-code input regression under ASan/UBSan")
