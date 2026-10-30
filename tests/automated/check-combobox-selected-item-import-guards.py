#!/usr/bin/env python3
"""Check ComboBox selected-item import rejects null and incomplete state."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
start = source.index("CBSetSelectedItem(Widget")
body = source[start:source.index("/*\n * CBGetSelectedItem", start)]

assert "new_value == NULL" in body
assert "CB_List(cb) == NULL" in body
assert "CB_EditBox(cb) == NULL" in body
assert "if (item == NULL)" in body
assert "XmListItemPos(CB_List(cb), new_value)" in body

print("ComboBox selected-item import rejects incomplete values")
