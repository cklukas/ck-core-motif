#!/usr/bin/env python3
"""Ensure ColorSelector SetValues validates widget and argument inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ColorS.c").read_text()
start = source.index("SetValues(Widget current, Widget request, Widget set,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "current == NULL || request == NULL || set == NULL || args == NULL" in body
assert "num_args == NULL" in body
assert body.count("XtIsSubclass") >= 3
assert body.index("if (current == NULL") < body.index("_XmFilterArgs")
print("ColorSelector SetValues validates widget and argument inputs")
