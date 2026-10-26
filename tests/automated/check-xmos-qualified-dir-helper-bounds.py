#!/usr/bin/env python3
"""Check GetQualifiedDir uses checked path-buffer sizing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("AllocatePathBuffer(size_t prefix_len")
body = source[start:source.index("String\n_XmOSFindPatternPart", start)]
assert "prefix_len > SIZE_MAX - suffix_len" in body
assert "total + extra > (size_t)UINT_MAX" in body
assert "AllocatePathBuffer(userDirLen, dirSpecLen, 2)" in body
assert "AllocatePathBuffer(userDirLen, dirSpecLen, 3)" in body
print("PASS qualified-directory helper allocation bounds")
