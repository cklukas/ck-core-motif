#!/usr/bin/env python3
"""Check ComboBox item-setting API rejects null before app-context lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text(encoding="utf-8")
start = source.index("XmComboBoxSetItem")
region = source[start:source.index("XmCombinationBoxGetValue", start)]
assert "if (widget == NULL)" in region
assert region.index("if (widget == NULL)") < region.index("_XmWidgetToAppContext(widget)")
print("PASS ComboBox item-setting API validates null widgets")
