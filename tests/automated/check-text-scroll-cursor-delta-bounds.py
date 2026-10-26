#!/usr/bin/env python3
"""Ensure ScrollCursorVertically bounds its int scroll delta."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nScrollCursorVertically(")
end = source.index("\nstatic void \nAddNewLine", start)
region = source[start:end]

assert "int64_t scroll_delta;" in region
assert "scroll_delta = (int64_t)cur_line - (int64_t)desired_line;" in region
assert "scroll_delta > INT_MAX" in region
assert "scroll_delta < INT_MIN" in region
assert "XmTextScroll(w, (int)scroll_delta);" in region
assert "XmTextScroll(w, (int)(cur_line - desired_line));" not in region

print("Text ScrollCursorVertically: line delta is bounded before scrolling")
