#!/usr/bin/env python3
"""Check the fallback XPM string duplicate rejects null input safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmmisc.c").read_text()
start = source.index("xpmstrdup(char *s1)")
end = source.index("#endif", start)
function = source[start:end]

assert "if (s1 == NULL)" in function
assert "return NULL;" in function
assert "l = strlen(s1);" in function
assert "if (l == SIZE_MAX)" in function
assert "l++;" in function

print("PASS XPM fallback string duplicate validates input")
