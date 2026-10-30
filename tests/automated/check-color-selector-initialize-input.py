#!/usr/bin/env python3
"""Ensure ColorSelector Initialize validates widget and argument inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ColorS.c").read_text()
start = source.index("Initialize(Widget request, Widget set, ArgList args,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "request == NULL || set == NULL || args == NULL" in body
assert "num_args == NULL" in body
assert "!XtIsSubclass(set, xmColorSelectorWidgetClass)" in body
assert body.index("if (request == NULL") < body.index("_XmFilterArgs")
print("ColorSelector Initialize validates widget and argument inputs")
