#!/usr/bin/env python3
"""Ensure TextField display-rectangle geometry is widened and guarded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("TextFieldGetDisplayRect(Widget w,")
end = source.index("\n\n\n/* ARGSUSED */\nstatic void\nTextFieldMarginsProc", start)
region = source[start:end]

assert "if (display_rect == NULL)" in region
assert "uint64_t margin_width" in region
assert "uint64_t margin_top" in region
assert "uint64_t margin_bottom" in region
assert "2 * margin_width" in region
assert "margin_top + margin_bottom" in region
assert "Position margin_width" not in region

print("TextField display rectangles use widened guarded arithmetic")
