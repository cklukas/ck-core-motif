#!/usr/bin/env python3
"""Check TimePicker range updates invalidate a realized widget."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TimePicker.c").read_text()
start = source.index("Boolean XmTimePickerSetRange")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "picker->time_picker.minimum = *minimum;" in body
assert "picker->time_picker.maximum = *maximum;" in body
invalidate = "if (XtIsRealized(widget))"
assert invalidate in body
assert "XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);" in body
assert body.index(invalidate) > body.index("picker->time_picker.maximum = *maximum;")
print("PASS TimePicker range updates invalidate realized widgets")
