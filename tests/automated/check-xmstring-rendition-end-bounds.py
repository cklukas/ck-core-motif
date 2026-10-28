#!/usr/bin/env python3
"""Ensure XmString rendition-end removal avoids signed tag underflow."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nSpecifiedSegmentExtents")
body = source[start:source.index("\nstatic void \n_parse_locale", start)]
assert "unsigned int\t\ti, j, depth, hits;" in body
assert "for (j = tag_count; j != 0; --j)" in body
assert "tags[j - 1]" in body
print("PASS XmString rendition-end tag traversal bounds")
