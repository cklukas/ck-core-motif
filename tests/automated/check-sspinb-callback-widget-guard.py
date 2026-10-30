#!/usr/bin/env python3
"""Check SimpleSpinBox callback-widget lookup validates its input."""

from pathlib import Path


source = Path("lib/Xm/SSpinB.c").read_text(encoding="utf-8")
start = source.rindex("GetCallbackWidget(Widget widget)")
end = source.index("\n}", start)
body = source[start:end]
assert "widget == NULL || !XtIsSubclass(widget, xmSimpleSpinBoxWidgetClass)" in body
assert "return NULL" in body
assert "ssb_w->simpleSpinBox.text_field" in body

print("SimpleSpinBox callback-widget guard present")
