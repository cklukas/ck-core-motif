#!/usr/bin/env python3
"""Check guarded growth of the XmString table resource converter."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")

if "if (table_size > INT_MAX / 2)" not in text:
    raise SystemExit("XmString table growth guard is missing")
if "for (cleanup = 0; cleanup < str_no; ++cleanup)" not in text:
    raise SystemExit("XmString table growth failure does not clean up")
if "return False;" not in text:
    raise SystemExit("XmString table growth failure does not return failure")

print("XmString table growth is bounded")
