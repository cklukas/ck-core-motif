#!/usr/bin/env python3
"""Check I18List row/column conversion has one pixel-y declaration."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("CvtPositionToRowColumn(Widget w")
body = source[start : source.index("/*\tFunction Name: CvtRowColumnToPosition", start)]
assert body.count("long long pixel_y") == 1
assert "long long pixel_y = y;" in body
print("PASS I18List row/column conversion has one pixel-y declaration")
