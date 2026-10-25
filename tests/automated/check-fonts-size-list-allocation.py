#!/usr/bin/env python3
"""Check FontS size and alternate-family list allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert "(size_t)(size + 1) > SIZE_MAX / sizeof(*strs)" in source
assert source.count("if (strs == NULL)") >= 1
assert "(size_t)num > SIZE_MAX / sizeof(*strs)" in source
assert "strs == NULL && num != 0" in source
print("PASS Font selector size-list allocations are guarded")
