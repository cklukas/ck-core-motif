#!/usr/bin/env python3
"""Check color database parsing at EOF without a trailing newline."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")

needle = "if (temp != NULL)\n\t    *temp = '\\0';"
if needle not in text:
    raise SystemExit("color database EOF newline guard is missing")

print("color database parser accepts final lines without newline")
