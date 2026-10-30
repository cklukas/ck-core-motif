#!/usr/bin/env python3
"""Check Form synthetic constraint-offset exports validate callback state."""

from pathlib import Path


source = Path("lib/Xm/Form.c").read_text()
for function in ("FromTopOffset", "FromBottomOffset", "FromLeftOffset", "FromRightOffset"):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL || value == NULL" in body, function
    assert "w->core.constraints == NULL" in body, function
    assert "XtIsSubclass(w->core.parent, xmFormWidgetClass)" in body, function

print("Form offset getters validate widget, parent, constraint, and output")
