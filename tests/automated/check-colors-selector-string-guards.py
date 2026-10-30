#!/usr/bin/env python3
"""Check ColorSelector synthetic string getters validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text(encoding="utf-8")
names = (
    "GetValues_XmNredSliderLabel", "GetValues_XmNgreenSliderLabel",
    "GetValues_XmNblueSliderLabel", "GetValues_XmNcolorListTogLabel",
    "GetValues_XmNsliderTogLabel", "GetValues_XmNnoCellError",
    "GetValues_XmNfileReadError",
)
for name in names:
    start = source.rindex(name)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL || value == NULL" in body
    assert "XtIsSubclass(w, xmColorSelectorWidgetClass)" in body
    assert "*value = (XtArgVal) NULL" in body

print("ColorSelector string getter guards present")
