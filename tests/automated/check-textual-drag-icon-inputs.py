#!/usr/bin/env python3
"""Check textual drag-icon lookup validates its widget before XtScreen."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragIcon.c").read_text(encoding="utf-8")
start = source.index("XmeGetTextualDragIcon(")
region = source[start:]
assert "if (w == NULL)" in region
assert region.index("if (w == NULL)") < region.index("screen = XtScreen(w)")
print("PASS XmeGetTextualDragIcon validates null widgets")
