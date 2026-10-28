#!/usr/bin/env python3
"""Check the text selection responder validates target before dereference."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmTextConvert(")
body = source[start : source.index("\n}", start) + 2]
assert "target == NULL" in body
guard = body.index("target == NULL")
first_target_deref = body.index("*target", guard)
assert guard < first_target_deref
print("PASS text selection responder guards null target")
