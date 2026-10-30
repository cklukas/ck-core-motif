#!/usr/bin/env python3
"""Ensure FontSelector SetValues validates widget and argument inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text()
start = source.index("SetValues(Widget old, Widget request, Widget set,")
end = source.index("\n} /* SetValues */", start)
body = source[start:end]

assert "old == NULL || request == NULL || set == NULL || args == NULL" in body
assert "num_args == NULL" in body
assert body.count("XtIsSubclass") >= 3
assert body.index("if (old == NULL") < body.index("_XmFilterArgs")
print("FontSelector SetValues validates widget and argument inputs")
