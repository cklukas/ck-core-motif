#!/usr/bin/env python3
"""Check usable-text table allocation handles non-empty tables."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("/* Get useable text for each compound string */")
body = source[start:source.index("for (i = 0, total_size", start)]
assert "useable_text = (char **)XtMalloc(" in body
assert "sizeof(char *) * (size_t)count" in body
assert "UINT_MAX / sizeof(char *)" in body
assert "if (count > 0 && useable_text == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS usable-text table allocation is guarded")
