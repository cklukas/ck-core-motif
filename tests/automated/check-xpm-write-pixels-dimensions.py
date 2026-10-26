#!/usr/bin/env python3
"""Check XPM pixel writing rejects zero dimensions before subtraction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
start = source.index("WritePixels(")
start = source.index("WritePixels(\n", start)
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "if (width == 0 || height == 0)" in body
assert "return XpmFileInvalid;" in body
assert body.index("if (width == 0 || height == 0)") < body.index("h = height - 1;")
print("PASS XPM pixel writing rejects zero dimensions")
