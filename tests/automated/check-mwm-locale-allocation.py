#!/usr/bin/env python3
"""Ensure MWM locale copies validate their Xt allocation width."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = text.rindex("FILE *FopenConfigFile (void)")
end = text.index("static Boolean", start)
function = text[start:end]
assert "langLength = strlen(LANGp)" in function
assert "langLength >= UINT_MAX" in function
assert "XtMalloc((Cardinal)(langLength + 1))" in function
assert "XtMalloc(strlen(LANGp) +1)" not in function
print("MWM locale allocation is width-checked")
