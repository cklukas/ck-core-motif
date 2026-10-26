#!/usr/bin/env python3
"""Check XmDragStart validates widget and event inputs before lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text(encoding="utf-8")
start = source.index("XmDragStart(")
region = source[start:]
assert "if (w == NULL || event == NULL)" in region
assert region.index("if (w == NULL || event == NULL)") < region.index("_XmWidgetToAppContext(w)")
assert region.index("if (w == NULL || event == NULL)") < region.index("event->type")
print("PASS XmDragStart validates null widget and event inputs")
