#!/usr/bin/env python3
"""Check DropDown popup placement stays wide until its X11 boundary."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nPopupList(Widget w)")
end = source.index("\nstatic ", start + 1)
body = source[start:end]

assert "static Position\ndropdown_bounded_position(long long value)" in source
assert "static Dimension\ndropdown_bounded_dimension(unsigned long value)" in source
assert "int x, y, temp;" in body
assert "width = dropdown_bounded_dimension((unsigned long)available_width);" in body
assert "(long long)y + XtHeight(shell)" in body
assert "(long long)x + width" in body
assert "XmNx, dropdown_bounded_position(x)" in body
assert "XmNy, dropdown_bounded_position(y)" in body
print("PASS DropDown popup placement bounds wide coordinates")
