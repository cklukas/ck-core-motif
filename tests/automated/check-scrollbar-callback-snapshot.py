#!/usr/bin/env python3
"""Check ScrollBar callback snapshots handle count and allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c").read_text()
start = source.index("CallScrollCallbackList(")
body = source[start:source.index("static void\nScrollCallback", start)]
assert "if (count == UINT_MAX)" in body
assert "SIZE_MAX / sizeof(*snapshot)" in body
assert "UINT_MAX / sizeof(*snapshot)" in body
assert "if (snapshot == NULL)" in body
print("PASS ScrollBar callback snapshot bounds and failure handling")
