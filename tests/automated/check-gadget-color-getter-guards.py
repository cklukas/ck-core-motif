#!/usr/bin/env python3
"""Check Gadget synthetic color getters validate widget and manager state."""

from pathlib import Path


source = Path("lib/Xm/Gadget.c").read_text(encoding="utf-8")
for name in ("GetHighlightColor(", "GetTopShadowColor(", "GetBottomShadowColor("):
    start = source.rindex(name)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL || value == NULL" in body
    assert "XtIsSubclass(w, xmGadgetClass)" in body
    assert "parent == NULL || !XmIsManager(parent)" in body
    assert "*value = (XtArgVal) 0" in body

print("Gadget color getter guards present")
