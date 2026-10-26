#!/usr/bin/env python3
"""Check that empty compound-text components do not copy NULL values."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")
needle = "memcpy((char*)tmp, val, len);"
if text.count("if (len != 0)\n\t    " + needle) != 2:
    raise SystemExit("compound-text component NULL guards missing")

print("empty compound-text components avoid NULL memcpy")
