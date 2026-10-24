#!/usr/bin/env python3
"""Check bounded accumulation while joining XmString text segments."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text(encoding="utf-8")

if "size_t size_so_far = 1;" not in text:
    raise SystemExit("usable-text accumulator is not size_t")
if "text_length > SIZE_MAX - size_so_far" not in text:
    raise SystemExit("usable-text overflow guard is missing")
if "return(XNoMemory);" not in text:
    raise SystemExit("usable-text overflow does not return XNoMemory")

print("usable-text concatenation length accounting is bounded")
