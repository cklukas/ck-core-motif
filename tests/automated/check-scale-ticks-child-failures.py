#!/usr/bin/env python3
"""Check scale ticks omit failed separator children from management."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScalTics.c").read_text(
    encoding="utf-8"
)
start = source.index("sep[sep_num] = XmCreateSeparatorGadget")
region = source[start:source.index("XtManageChildren(sep, sep_num)", start)]
assert region.count("if (sep[sep_num] != (Widget) NULL)") == 4
assert region.count("sep_num++") == 4
print("scale tick generation excludes failed separator children")
