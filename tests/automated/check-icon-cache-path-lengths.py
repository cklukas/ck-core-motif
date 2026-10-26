#!/usr/bin/env python3
"""Check icon-cache path matching keeps source lengths in size_t."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("static int\nCheckDirCache")
end = source.index("static String\nfind_slash", start)
function = source[start:end]

assert "size_t\tdirNameLen, fileNameLen;" in function
assert "size_t\t\tcurrNameLen;" in function
assert "dirNameLen == SIZE_MAX || dirNameLen >= UINT_MAX" in function
assert "currNameLen = (currDir->nameOffsets[j + 1] -" in function
assert "strncmp(currName, filePtr, currNameLen)" in function

print("PASS icon-cache path length matching preserves size_t widths")
