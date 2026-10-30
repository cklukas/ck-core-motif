#!/usr/bin/env python3
"""Check NumberBox resource value updates clear stale invalid state."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("\n}\n", start) + 2
body = source[start:end]

value_change = "if (old->number_box.value != number->number_box.value)"
assert value_change in body
clear = "number->number_box.invalid = False;"
assert clear in body
assert body.index(clear) > body.index(value_change)
assert body.index("format_value(number);") > body.index(clear)
range_start = source.index("void XmNumberBoxSetRange")
range_end = source.index("\n}\n", range_start) + 2
range_body = source[range_start:range_end]
assert clear in range_body
assert range_body.index(clear) < range_body.index("format_value(number);")
print("PASS NumberBox resource value updates clear invalid state")
