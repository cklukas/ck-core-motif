#!/usr/bin/env python3
"""Check Xpm's fallback case-folding handles bytes safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(encoding="utf-8")
start = source.index("xpmstrcasecmp(\n")
end = source.index("xpmVisualType(", start)
region = source[start:end]
assert "u1 = (unsigned char)*s1;" in region
assert "u2 = (unsigned char)*s2;" in region
assert "tolower(u1)" in region and "tolower(u2)" in region
assert "tolower(*s1)" not in region and "tolower(*s2)" not in region
print("PASS Xpm fallback case-folding uses unsigned bytes")
