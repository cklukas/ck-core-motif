#!/usr/bin/env python3
"""Check DatePicker range updates invalidate a realized widget."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text()
start = source.index("Boolean XmDatePickerSetRange")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "picker->date_picker.minimum = *minimum;" in body
assert "picker->date_picker.maximum = *maximum;" in body
invalidate = "if (XtIsRealized(widget))"
assert invalidate in body
assert "XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);" in body
assert body.index(invalidate) > body.index("picker->date_picker.maximum = *maximum;")
print("PASS DatePicker range updates invalidate realized widgets")
