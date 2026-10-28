#!/usr/bin/env python3
"""Ensure Mrm value conversion validates its destination pointer."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_ConvertValue (Widget")
body = text[start:text.index("\n}\n\n\n", start)]
guard_pos = body.index("if (val == NULL)")
switch_pos = body.index("switch ( reptype )", guard_pos)
assert guard_pos < switch_pos
print("Mrm value conversion validates its destination pointer")
