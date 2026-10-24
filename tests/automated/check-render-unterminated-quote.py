#!/usr/bin/env python3
"""Check safe position handling for unterminated render-table strings."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

if "closed = (string[pos + count] == '\"');" not in text:
    raise SystemExit("render lexer quote-termination check is missing")
if "pos += closed ? end_offset + 1 : end_offset;" not in text:
    raise SystemExit("render lexer can advance past unterminated input")

print("render lexer stops unterminated strings at NUL")
