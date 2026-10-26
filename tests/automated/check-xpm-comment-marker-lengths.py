#!/usr/bin/env python3
"""Ensure XPM comment marker lengths retain strlen's size_t width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nParseComment")
end = source.index("\n/*\n * skip to the end", start)
region = source[start:end]
assert "size_t begin_len = strlen(begin);" in region
assert "size_t end_len = end ? strlen(end) : 0;" in region
assert "size_t i;" in region
assert "unsigned int begin_len" not in region
assert "unsigned int end_len" not in region
print("PASS XPM comment marker lengths preserve size_t width")
