#!/usr/bin/env python3
"""Check SimpleSpinBox synthetic getter validates widget and child state."""

from pathlib import Path


source = Path("lib/Xm/SSpinB.c").read_text()
start = source.rindex("SyntheticGetValue(Widget widget")
end = source.index("\n}", start)
body = source[start:end]
assert "value == NULL" in body
assert "XtIsSubclass(widget, xmSimpleSpinBoxWidgetClass)" in body
assert "text_field == NULL" in body
assert "*value = (XtArgVal) 0" in body

print("SimpleSpinBox synthetic getter validates widget, child, and output")
