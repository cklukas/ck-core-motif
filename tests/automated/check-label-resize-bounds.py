#!/usr/bin/env python3
"""Check Label Resize saturates natural dimensions and X11 coordinates."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nResize(Widget wid)")
end = source.index("static void \nInitialize(", start)
body = source[start:end]

assert "static Position\nlabel_bounded_position(long long value)" in source
assert "static Dimension\nlabel_bounded_dimension(unsigned long value)" in source
assert "newlw->core.width = label_bounded_dimension(" in body
assert "newlw->core.height = label_bounded_dimension(" in body
assert body.count("label_bounded_position(") >= 8
assert "lp->margin_left = label_bounded_dimension(" in body
assert "lp->margin_right = label_bounded_dimension(" in body
print("PASS Label Resize bounds natural geometry and drawing positions")
