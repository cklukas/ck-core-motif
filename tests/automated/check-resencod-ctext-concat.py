#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ResEncod.c").read_text()
start = source.rindex("ctextConcat(")
region = source[start:source.index("\n}\n", start) + 2]

assert "SIZE_MAX - (size_t)str2len - 1" in region
assert "> UINT_MAX" in region
assert "OctetPtr new_str" in region
assert "new_str = (OctetPtr)XtRealloc" in region
assert "if (new_str == NULL)" in region
assert region.index("if (new_str == NULL)") < region.index("memcpy")

print("Compound-text concatenation preserves the original buffer on growth failure")
