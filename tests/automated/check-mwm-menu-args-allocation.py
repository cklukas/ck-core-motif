#!/usr/bin/env python3
"""Ensure MWM menu argument allocations validate formatted lengths."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)
start = text.index("MakeMenuFunctionArgs(CmdTree *tree")
end = text.index("\n}\n", start)
function = text[start:end]
assert function.count("length >= 0 && length < UINT_MAX") == 2
assert function.count("XtMalloc((Cardinal)length + 1)") == 2
assert "XtMalloc((unsigned int)length + 1)" not in function
print("MWM menu argument allocations are width-checked")
