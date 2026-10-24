#!/usr/bin/env python3
"""Check that long alphabetic render-table tokens are not truncated."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

if "char temp[80]" in text:
    raise SystemExit("render lexer still uses a fixed 80-byte token buffer")
if "new_token -> string = (char *)XtMalloc((Cardinal)count + 1);" not in text:
    raise SystemExit("render lexer does not allocate token length")

print("render lexer preserves long alphabetic tokens")
