#!/usr/bin/env python3
"""Check DropDown's modern-style frame uses bounded drawing geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nExposeMethod(Widget wid, XEvent *event, Region r)")
end = source.index("\n/************************************************************", start)
body = source[start:end]

assert "int x, y;" in body
assert "unsigned long frame_width, frame_height;" in body
assert "frame_width = (unsigned long)text->core.width + arrow->core.width" in body
assert "w = dropdown_bounded_dimension(frame_width);" in body
assert "h = dropdown_bounded_dimension(frame_height);" in body
assert "dropdown_bounded_position(x), dropdown_bounded_position(y), w, h" in body
print("PASS DropDown frame drawing bounds geometry")
