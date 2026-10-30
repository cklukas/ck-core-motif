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
assert "source == NULL || source->data == NULL" in region
assert "data->widgets == NULL || data->numwidgets <= 0" in region
assert "data->widgets[0] == NULL" in region
assert "int oldleft, oldright" not in region

get_start = source.index("static Boolean \nGetSelection(")
get_end = source.index("\nstatic void \nSetSelection", get_start)
get_region = source[get_start:get_end]
assert "left == NULL || right == NULL" in get_region
assert "source == NULL || source->data == NULL" in get_region
assert "*left = *right = 0" in get_region

print("Text selection updates guard unattached sources and preserve wide positions")
