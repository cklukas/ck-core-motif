#!/usr/bin/env python3
"""Check ComboBox synthetic callbacks reject non-ComboBox widgets."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text(encoding="utf-8")
names = (
    "CBSetSelectedItem", "CBGetSelectedItem", "CBSetSelectedPos",
    "CBGetSelectedPos", "CBGetColumns", "CBGetItems", "CBGetItemCount",
    "CBGetVisibleItemCount",
)
for name in names:
    start = source.rindex(name + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "XtIsSubclass(widget, xmComboBoxWidgetClass)" in body

print("ComboBox callback class guards present")
