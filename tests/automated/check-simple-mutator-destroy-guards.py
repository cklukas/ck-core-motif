#!/usr/bin/env python3
"""Ensure repainting mutators do not operate during Xt destruction."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
functions = {
    "Prompt.c": ("void XmPromptSetVisible", "Boolean XmPromptGetVisible",
                 "_XmPaintInvalidate"),
    "DropHighlight.c": ("Boolean XmDropHighlightSetState",
                        "unsigned char XmDropHighlightGetState", "XClearArea"),
}
for filename, (start_marker, end_marker, redraw) in functions.items():
    source = (root / filename).read_text()
    start = source.index(start_marker)
    body = source[start:source.index(end_marker, start)]
    assert "widget->core.being_destroyed" in body, filename
    assert body.index("widget->core.being_destroyed") < body.index(redraw), filename

print("PASS simple repainting mutators reject destroying widgets")
