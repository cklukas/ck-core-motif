#!/usr/bin/env python3
"""Check null validation at the render-table property converter boundary."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c"
text = source.read_text(encoding="utf-8")

if "if (w == NULL || prop == NULL || len > (unsigned int)INT_MAX)" not in text:
    raise SystemExit("render converter null-input guard is missing")
if "return NULL;" not in text:
    raise SystemExit("render converter null input does not fail safely")

print("render-table converter rejects null widget or property input")
