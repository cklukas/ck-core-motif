#!/usr/bin/env python3
"""Check icon-cache invalidation keeps path lengths in size_t."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("void\nXmeFlushIconFileCache")
end = source.index("#ifndef XTHREADS", start)
function = source[start:end]

assert "size_t\tdirNameLen;" in function
assert "dirNameLen = strlen(path);" in function
assert "strncmp(currDir->dirName, path, dirNameLen)" in function

print("PASS icon-cache flush path length preserves size_t width")
