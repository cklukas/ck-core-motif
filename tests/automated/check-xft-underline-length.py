#!/usr/bin/env python3
"""Check that Xft UTF-16 underline extents receive character counts."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
assert "(FcChar16*)a, begin / 2, &ext" in source
assert "(FcChar16*)b, under_seg_len / 2, &ext" in source
print("PASS Xft UTF-16 underline extents use character counts")
