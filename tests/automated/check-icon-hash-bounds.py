#!/usr/bin/env python3
"""Check icon-name hashing bounds unsigned mixing before signed conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text(encoding="utf-8")
start = source.index("HashIconName (XmHashKey key)")
end = source.index("XmGetIconFileName(", start)
region = source[start:end]
assert "(unsigned int)INT_MAX" in region
assert "return (XmHashValue)(((((folded_len << 8)" in region
print("PASS icon-name hash bounds unsigned mixing before conversion")
