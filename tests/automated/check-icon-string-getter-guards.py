#!/usr/bin/env python3
"""Check IconGadget and IconButton compound-string exports validate inputs."""

from pathlib import Path


for path, function, class_name in (
    ("lib/Xm/IconG.c", "GetLabelString", "xmIconGadgetClass"),
    ("lib/Xm/IconButton.c", "GetValues_labelString", "xmIconButtonWidgetClass"),
):
    source = Path(path).read_text()
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "value == NULL" in body, function
    assert "XtIsSubclass" in body, function
    assert class_name in body, function

print("Icon compound-string getters validate widget and output pointers")
