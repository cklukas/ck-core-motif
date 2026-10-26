#!/usr/bin/env python3
"""Check ColorObj's screen-name table fits XmStackAlloc's unsigned size."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ColorObj.c").read_text(
    encoding="utf-8"
)

assert "SIZE_MAX / sizeof(char *)" in source
assert "UINT_MAX / sizeof(char *)" in source
print("ColorObj screen-name table allocation is bounded")
