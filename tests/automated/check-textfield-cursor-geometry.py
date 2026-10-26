#!/usr/bin/env python3
"""Ensure TextField cursor painting honors geometry-conversion failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("PaintCursor(XmTextFieldWidget tf)")
end = source.index("\nstatic void \nBlinkInsertionPoint", start)
region = source[start:end]

assert "if (!GetXYFromPos(tf, position, &x, &y)) return;" in region
assert region.index("position = TextF_CursorPosition(tf);") < region.index(
    "_XmTextFToggleCursorGC((Widget)tf);"
)
assert "(void) GetXYFromPos(tf, position, &x, &y);" not in region

print("TextField cursor painting rejects unrepresentable geometry")
