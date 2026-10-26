#!/usr/bin/env python3
"""Ensure Text selection updates handle unattached sources and wide positions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nSetSelection(")
end = source.index("\n/* Check the source method identity", start)
region = source[start:end]

assert "XmTextPosition oldleft, oldright;" in region
assert "data == NULL || data->widgets == NULL || data->numwidgets <= 0" in region
assert "data->widgets[0] == NULL" in region
assert "int oldleft, oldright" not in region

print("Text selection updates guard unattached sources and preserve wide positions")
