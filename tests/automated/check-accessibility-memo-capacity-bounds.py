#!/usr/bin/env python3
"""Check accessibility memo-table resize validates XtCalloc capacity."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
start = source.index("static Boolean memo_resize")
end = source.index("static TransportState *memo_get", start)
body = source[start:end]

assert "capacity == 0" in body
assert "capacity > (size_t)UINT_MAX / sizeof(*table)" in body
assert body.index("if (capacity == 0") < body.index("XtCalloc")
print("accessibility memo-table capacity is checked at allocation")
