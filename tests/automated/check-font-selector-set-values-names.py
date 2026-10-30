#!/usr/bin/env python3
"""Ensure FontSelector SetValues preflights resource names."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text()
start = source.index("SetValues(Widget old, Widget request, Widget set,")
end = source.index("\n} /* SetValues */", start)
body = source[start:end]

assert "if (args[i].name == NULL)" in body
assert body.index("if (args[i].name == NULL)") < body.index("_XmFilterArgs")
print("FontSelector SetValues preflights resource names")
