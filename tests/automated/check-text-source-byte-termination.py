#!/usr/bin/env python3
"""Ensure one-byte text conversion honors its terminator flag."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/TextStrSo.c").read_text()
start = source.index("case 1: {")
end = source.index("} /* end case 1 */", start)
body = source[start:end]
assert "count = num_chars;" in body
assert "if (add_null_terminator == True)" in body
assert "characters[count] = '\\0';" in body
print("One-byte text conversion honors add_null_terminator")
