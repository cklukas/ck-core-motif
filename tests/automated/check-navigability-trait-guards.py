#!/usr/bin/env python3
"""Check Manager/Primitive/Gadget navigability traits validate widgets."""

from pathlib import Path


checks = (
    ("lib/Xm/Manager.c", "xmManagerWidgetClass"),
    ("lib/Xm/Primitive.c", "xmPrimitiveWidgetClass"),
    ("lib/Xm/Gadget.c", "xmGadgetClass"),
)
for filename, widget_class in checks:
    source = Path(filename).read_text(encoding="utf-8")
    start = source.rindex("WidgetNavigable(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL" in body
    assert f"XtIsSubclass(wid, {widget_class})" in body
    assert "return XmNOT_NAVIGABLE" in body

print("Navigability trait guards present")
