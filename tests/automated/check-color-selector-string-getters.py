#!/usr/bin/env python3
"""Ensure ColorSelector string getters reject missing internal strings."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ColorS.c").read_text()
start = source.rindex("static void GetValues_XmNredSliderLabel")
end = source.index("/*      Function Name:", start)
body = source[start:end]

for needle in (
    "slider_labels[0] == NULL",
    "slider_labels[1] == NULL",
    "slider_labels[2] == NULL",
    "tog_labels[0] == NULL",
    "tog_labels[1] == NULL",
    "no_cell_error == NULL",
    "file_read_error == NULL",
):
    assert needle in body, needle
print("ColorSelector string getters validate internal strings")
