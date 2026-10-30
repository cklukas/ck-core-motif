#!/usr/bin/env python3
"""Check LabelGadget synthetic string getters validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/LabelG.c").read_text()
for function in (
    "GetLabelString",
    "GetAccelerator",
    "GetAcceleratorText",
    "GetMnemonicCharset",
):
    start = source.rindex(function + "(")
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL" in body, function
    assert "value == NULL" in body, function
    assert "XmIsLabelGadget(wid)" in body, function

print("LabelGadget synthetic string getters validate widget and output pointers")
