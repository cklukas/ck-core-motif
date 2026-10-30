#!/usr/bin/env python3
"""Check Label synthetic string getters validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Label.c").read_text()
for function in (
    "GetLabelString",
    "GetAccelerator",
    "GetAcceleratorText",
    "GetMnemonicCharSet",
):
    start = source.rindex(function + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL" in body, function
    assert "value == NULL" in body, function
    assert "XtIsSubclass(wid, xmLabelWidgetClass)" in body, function

print("Label synthetic string getters validate widget and output pointers")
