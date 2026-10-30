#!/usr/bin/env python3
"""Check ComboBox selected-position import validates its converter input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("static XmImportOperator\nCBSetSelectedPos")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (widget == NULL || value == NULL ||" in body
assert "!XtIsSubclass(widget, xmComboBoxWidgetClass)" in body
assert body.index("if (widget == NULL || value == NULL ||") < body.index("cb->composite.num_children")
assert "new_pos == XmINVALID_POSITION || new_pos < 0" in body
assert "new_pos != XmINVALID_POSITION && new_pos < 1" in body
assert body.index("new_pos == XmINVALID_POSITION || new_pos < 0") < body.index("new_pos++")

print("ComboBox selected-position import guards null converter input")
