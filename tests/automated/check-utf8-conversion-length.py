#!/usr/bin/env python3
"""Check that UTF-8 resource conversion accounts for converted bytes."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")

needle = "*outlen += (unsigned int)converted_len;"
if needle not in text:
    raise SystemExit("UTF-8 conversion does not advance by converted length")

old = "*outlen += ctlen;\n    XtFree(text);"
if old in text:
    raise SystemExit("UTF-8 conversion still advances by source length")

if "converted_len > UINT_MAX" not in text:
    raise SystemExit("UTF-8 conversion length overflow guard is missing")

print("UTF-8 conversion length accounting is bounded")
