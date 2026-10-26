#!/usr/bin/env python3
"""Ensure the legacy menu-accelerator getter bounds its copy."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmRC_GetMenuAccelerator(")
end = text.index("\n/*ARGSUSED*/", start)
block = text[start:end]

assert "size_t        accelerator_length;" in block
assert "accelerator_length > (size_t)UINT_MAX - 1" in block
assert "XtMalloc((Cardinal)(accelerator_length + 1))" in block
assert "*value = (XtArgVal) NULL;" in block

print("legacy menu-accelerator copy bounds: ok")
