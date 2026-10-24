#!/usr/bin/env python3
"""Check defined ctype inputs in the render-table lexer."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

for needle in (
    "isspace((unsigned char)string[pos])",
    "isalpha((unsigned char)string[pos])",
    "isalpha((unsigned char)string[pos + count])",
):
    if needle not in text:
        raise SystemExit(f"render lexer ctype guard missing: {needle}")

print("render-table lexer ctype conversion is defined for all bytes")
