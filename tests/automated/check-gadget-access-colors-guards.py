#!/usr/bin/env python3
"""Check Gadget access-colors trait validates widget and output state."""

from pathlib import Path


source = Path("lib/Xm/Gadget.c").read_text(encoding="utf-8")
start = source.rindex("GetColors(Widget w")
end = source.index("\n}", start)
body = source[start:end]
assert "w == NULL || color_data == NULL" in body
assert "XtIsSubclass(w, xmGadgetClass)" in body
assert "parent_widget == NULL || !XmIsManager(parent_widget)" in body
assert "color_data->valueMask" in body

print("Gadget access-colors guards present")
