#!/usr/bin/env python3
"""Check guarded growth of the string-table resource converter."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")

if "if (size >= INT_MAX" not in text:
    raise SystemExit("empty-entry string table growth guard is missing")
if "if (size > INT_MAX / 2 ||" not in text:
    raise SystemExit("string table doubling guard is missing")
if text.count("for (cleanup = 0; cleanup < i; ++cleanup)") < 2:
    raise SystemExit("string table growth failure cleanup is incomplete")

print("string-table growth is bounded")
