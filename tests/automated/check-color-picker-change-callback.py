#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/ColorPicker.c").read_text()
s = "".join(source.split())
body=s[s.index("voidXmColorPickerSetValue"):s.index("voidXmColorPickerPreviewValue")]
assert "Booleanchanged=p->color_picker.value!=(uint32_t)value" in body
assert "if(changed&&notify_change)notify(" in body
print("PASS color-picker-change-callback")
