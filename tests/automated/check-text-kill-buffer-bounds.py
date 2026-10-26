#!/usr/bin/env python3
"""Ensure Text kill-buffer snapshots are checked before Xlib conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean \nDeleteOrKill(")
end = source.index("\nstatic void \nStuffFromBuffer", start)
region = source[start:end]

assert "size_t ptr_length;" in region
assert "if (ptr == NULL)" in region
assert "ptr_length = strlen(ptr);" in region
assert "ptr_length > (size_t)INT_MAX" in region
assert "XStoreBuffer(XtDisplay(tw), ptr, (int)ptr_length, 0);" in region

print("Text DeleteOrKill: kill-buffer allocation and length are bounded")
