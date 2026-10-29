#!/usr/bin/env python3
"""Check ColorPicker value callbacks are emitted only for actual changes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text()
start = source.index("void XmColorPickerSetValue")
end = source.index("void XmColorPickerPreviewValue", start)
function = source[start:end]
assert "Boolean changed;" in function
assert "changed = picker->color_picker.value != value;" in function
assert "if (notify_change && changed)" in function
print("ColorPicker value callbacks require an actual committed-value change")
