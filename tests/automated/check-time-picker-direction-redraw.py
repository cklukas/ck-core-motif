#!/usr/bin/env python3
"""Check a TimePicker requests repaint after a layout-direction update."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TimePicker.c").read_text()
start = source.index("static Boolean SetValues(")
end = source.index("static void Redisplay(", start)
body = source[start:end]

assert "Boolean direction_changed = old->primitive.layout_direction !=" in body
assert "picker->primitive.layout_direction;" in body
assert "||\n           direction_changed;" in body
print("PASS TimePicker direction updates request redraw")
