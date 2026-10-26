#!/usr/bin/env python3
"""Check dialog construction avoids zero-length memcpy from a null list."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DialogS.c").read_text(
    encoding="utf-8"
)
start = source.rindex("XmeCreateClassDialog(")
body_start = source.index("{", start)
body_end = source.index("\n}", body_start)
body = source[body_start:body_end]
assert "if (bb_n != 0)\n\tmemcpy(ds_args, bb_args, sizeof(Arg) * (size_t)bb_n);" in body
assert body.index("if (bb_n != 0)") < body.index("XtSetArg( ds_args[bb_n]")
print("dialog construction skips zero-length memcpy from a null argument list")
