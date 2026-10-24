#!/usr/bin/env python3
"""Check safe numeric token conversion in the render-table lexer."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

if "if (newpos == &string[pos])" not in text:
    raise SystemExit("render lexer no-conversion guard is missing")
if "floor(result) == result" not in text:
    raise SystemExit("render lexer integer range check is missing")
if "result >= (double)INT_MIN" not in text:
    raise SystemExit("render lexer integer lower bound is missing")

print("render lexer handles invalid and out-of-range numeric tokens")
