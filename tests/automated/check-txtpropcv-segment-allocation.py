#!/usr/bin/env python3
"""Check GetTextSegment handles all temporary buffer allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("GetTextSegment(")
body = source[start:source.index("/************************************************************************", start + 1)]
assert body.count("if (tmp == NULL)") == 3
assert body.count("return(_INVALID_SEGMENT);") >= 4
assert "if (newstring == NULL)" in body
assert "XtFree(*buffer);" in body
assert "*buffer = newstring;" in body
print("PASS text segment temporary allocations are guarded")
