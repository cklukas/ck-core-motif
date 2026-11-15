#!/usr/bin/env python3
"""Compile the WSM CARD16 list limit with its protocol type."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile


root = Path(__file__).resolve().parents[2]
source = (root / "clients/mwm/WmWsmLib/pack.c").read_text(encoding="utf-8")
assert "#define WSM_MAX_LIST_ITEMS ((int)(CARD16)~(CARD16)0)" in source
compiler = shutil.which(os.environ.get("CC", "cc"))
assert compiler is not None

fixture = """\
#if __has_include(<X11/Xmd.h>)
#include <X11/Xmd.h>
#else
#include <stdint.h>
typedef uint16_t CARD16;
#endif
#define WSM_MAX_LIST_ITEMS ((int)(CARD16)~(CARD16)0)
int main(void) { return WSM_MAX_LIST_ITEMS == 65535 ? 0 : 1; }
"""
extra_includes = (
    ["-I/opt/X11/include"]
    if os.access("/opt/X11/include/X11/Xmd.h", os.R_OK)
    else []
)
with tempfile.TemporaryDirectory(dir=os.environ.get("TMPDIR")) as directory:
    path = Path(directory)
    program = path / "wsm-card16-limit"
    subprocess.run(
        [compiler, "-std=c99", "-Wall", "-Wextra", "-Werror", *extra_includes, "-x", "c", "-", "-o", str(program)],
        input=fixture,
        text=True,
        check=True,
    )
    subprocess.run([str(program)], check=True)

print("PASS WSM CARD16 list maximum is 65535")
