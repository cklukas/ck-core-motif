#!/usr/bin/env python3
"""Ensure ClearSelection validates range width and returned storage."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nClearSelection(Widget w,")
end = source.index("\nstatic void \nProcessBDragRelease", start)
region = source[start:end]

assert "XmTextPosition span;" in region
assert "right - left > INT_MAX" in region
assert "select_string = _XmStringSourceGetString" in region
assert "if (select_string == NULL)" in region
assert "num_spaces = (int)span;" in region

print("Text ClearSelection: range and temporary-string failures are handled")
