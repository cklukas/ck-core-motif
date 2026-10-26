#!/usr/bin/env python3
"""Check TextOut line wrapping validates source progress."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
start = source.index("_XmTextFindLineEnd")
end = source.index("\nstatic XtGeometryResult", start)
body = source[start:end]

assert body.count("next_position <= position") >= 3
assert body.count("block.length > 0 && block.ptr == NULL") >= 3
assert "return PASTENDPOS;" in body

print("TextOut line wrapping guards source progress")
