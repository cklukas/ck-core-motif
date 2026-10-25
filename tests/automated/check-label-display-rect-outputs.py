#!/usr/bin/env python3
"""Check that Label gadget display-rectangle outputs are validated."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
needle = "if (w == NULL || displayrect == NULL)"
if needle not in text:
    raise SystemExit("Label display rectangle output guard is missing")

print("Label gadget display rectangle output guard present")
