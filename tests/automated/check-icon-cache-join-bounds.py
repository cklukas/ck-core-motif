#!/usr/bin/env python3
"""Check cached icon directory/leaf joining uses checked size_t arithmetic."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c"
text = source.read_text(encoding="utf-8")
start = text.index("size_t dirLen, leafLen;")
end = text.index("\n\tif (fileName)", start)
region = text[start:end]
assert "size_t dirLen, leafLen" in region
assert "size_t pathLen" in region
assert "dirLen <= SIZE_MAX - 2" in region
assert "leafLen <= SIZE_MAX - dirLen - 2" in region
assert "pathLen <= UINT_MAX" in region
assert "XtMalloc((Cardinal)pathLen)" in region
assert "if (fileName != NULL)" in region
print("cached icon path joining bounds lengths and allocation")
