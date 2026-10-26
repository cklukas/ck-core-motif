#!/usr/bin/env python3
"""Ensure line-position helpers bound large metric arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
for name, next_name in (("XtoPosInLine", "YtoPosInLine"), ("YtoPosInLine", None)):
    start = source.index(f"static XmTextPosition\n{name}(")
    if next_name is None:
        region = source[start:]
    else:
        end = source.index(f"\n\nstatic XmTextPosition\n{next_name}(", start)
        region = source[start:end]
    assert "int64_t         coordinate;" in region
    assert "coordinate > INT_MAX" in region
    assert "coordinate < INT_MIN" in region
    assert "ClampTextPointCoordinate((int)coordinate)" in region

print("Text line coordinates: metric arithmetic is bounded before hit-testing")
