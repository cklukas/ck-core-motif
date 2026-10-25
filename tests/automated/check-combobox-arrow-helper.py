#!/usr/bin/env python3
"""Check ComboBox arrow helpers guard the plain ComboBox type."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
for name in ("SetHitArea(Widget w)", "DrawArrow(Widget widget,"):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "CB_Type(cb) == XmCOMBO_BOX" in region
    assert "return;" in region
    assert "assert(CB_Type(cb) != XmCOMBO_BOX);" not in region

print("ComboBox arrow helpers guard plain ComboBoxes")
