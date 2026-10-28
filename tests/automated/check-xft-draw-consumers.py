#!/usr/bin/env python3
"""Ensure Xft draw consumers guard failed creation."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
render = (root / "XmRenderT.c").read_text(encoding="utf-8")
assert "if (draw == NULL)\n\treturn;" in render
assert "if (d == NULL)\n\t\treturn;" in render
for name in ("TextOut.c", "TextF.c", "Label.c", "LabelG.c", "XmString.c"):
    text = (root / name).read_text(encoding="utf-8")
    if "_XmXftDrawCreate" in text:
        assert "draw != NULL" in text or "draw == NULL" in text, name
print("Xft draw consumers guard creation failure")
