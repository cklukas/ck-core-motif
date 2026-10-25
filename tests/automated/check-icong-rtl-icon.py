#!/usr/bin/env python3
"""Check exported IconG RTL icon positions use signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "(int) XtWidth(wid) - (int) icon_x -" in source
assert "(int) IG_SmallIconRectWidth(wid)" in source
assert "(int) IG_LargeIconRectWidth(wid)" in source

print("IconG RTL icon placement is signed")
