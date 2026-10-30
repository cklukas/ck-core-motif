#!/usr/bin/env python3
"""Check Notebook orientation pixel hooks validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Notebook.c").read_text()
for function in (
    "FromOrientationPixels",
    "ToOrientationPixels",
    "FromOrientationOppositePixels",
    "ToOrientationOppositePixels",
):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL" in body, function
    assert "value == NULL" in body, function
    assert "XtIsSubclass(w, xmNotebookWidgetClass)" in body, function

print("Notebook orientation pixel hooks validate widget and value inputs")
