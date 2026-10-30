#!/usr/bin/env python3
"""Check LabelGadget padding converters validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/LabelG.c").read_text()
for function in ("FromPaddingPixels", "ToPaddingPixels"):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "widget == NULL || value == NULL" in body, function
    assert "XtIsSubclass(widget, xmLabelGadgetClass)" in body, function

assert "return XmSYNTHETIC_NONE" in source
print("LabelGadget padding converters validate widget and value inputs")
