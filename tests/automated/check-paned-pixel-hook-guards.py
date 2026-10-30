#!/usr/bin/env python3
"""Check Paned orientation and child pixel hooks validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Paned.c").read_text()
for function in (
    "_XmFromPanedPixels",
    "FromPanedChildPixels",
    "_XmToPanedPixels",
    "ToPanedChildPixels",
    "FromPanedOppositePixels",
    "ToPanedOppositePixels",
):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "widget == NULL || value == NULL" in body, function
    assert "xmPanedWidgetClass" in body, function

print("Paned pixel hooks validate widget, parent, and value inputs")
