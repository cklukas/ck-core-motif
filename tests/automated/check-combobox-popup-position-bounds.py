#!/usr/bin/env python3
"""Check ComboBox popup positioning stays wide until the X11 boundary."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nCBDropDownList")
end = source.index("static void\nCBFocusIn", start)
body = source[start:end]

assert "static Position\nbounded_position(long long value)" in source
assert "int shell_x, shell_y;" in body
assert "shell_x = (int)root_x - (int)XtX(cb)" in body
assert "shell_y = (int)root_y + (int)XtHeight(cb)" in body
assert "XmNx, bounded_position(shell_x)" in body
assert "XmNy, bounded_position(shell_y)" in body
print("PASS ComboBox popup positioning clamps wide coordinates")
