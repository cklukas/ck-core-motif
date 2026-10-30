#!/usr/bin/env python3
"""Check Gadget tooltip synthetic hooks validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Gadget.c").read_text()
for signature in ("GetToolTipString(Widget wid", "SetToolTipString(Widget wid"):
    start = source.rindex(signature)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL || value == NULL" in body, signature

assert "*value = (XtArgVal) NULL" in source
assert "return XmSYNTHETIC_NONE" in source
print("Gadget tooltip synthetic hooks validate widget and value inputs")
