#!/usr/bin/env python3
"""Check XPM pixel writing rejects zero/short color tokens safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
start = source.index("WritePixels(")
region = source[start : source.index("WriteExtensions(\n", start)]

assert "cpp == 0" in region
assert region.count("token_length = strlen(colors[*pixels].string);") == 2
assert region.count("if (token_length < cpp)") == 2
assert region.count("memcpy(s, colors[*pixels].string, cpp);") == 2
assert "strncpy(s, colors[*pixels].string, cpp)" not in region
print("XPM pixel writing rejects zero and short color tokens")
