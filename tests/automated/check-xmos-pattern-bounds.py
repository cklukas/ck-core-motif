#!/usr/bin/env python3
"""Check that Xmos pattern conversion guards null and size-overflow input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("static String\nGetFixedMatchPattern(String pattern)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (pattern == NULL)" in region
assert "pattern_len > (SIZE_MAX / 2) - 4" in region
assert "XtCalloc(2, pattern_len + 4)" in region
assert "XtCalloc(2, strlen(pattern) + 4)" not in region
print("Xmos pattern conversion guards input and allocation arithmetic")
