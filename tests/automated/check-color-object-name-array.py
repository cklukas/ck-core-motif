#!/usr/bin/env python3
"""Check that color-object atom-name allocation checks its product."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = source.index("/* Intern all the atoms in a single round-trip. */")
end = source.index("#undef MAX_SCREENS", start)
region = source[start:end]
assert "SIZE_MAX / sizeof(char *)" in region
assert "XmStackAlloc((size_t)nscreens * sizeof(char *)" in region
print("PASS color-object atom-name allocation checks its product")
