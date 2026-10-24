#!/usr/bin/env python3
"""Check that empty quoted render-table tokens advance the lexer."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

if 'new_token -> string = XtNewString("");' not in text:
    raise SystemExit("empty quoted token does not produce an empty string")
if "pos += count + 2; /* Move past end quote */" not in text:
    raise SystemExit("empty quoted token does not advance past the quote")

print("render lexer advances empty quoted tokens")
