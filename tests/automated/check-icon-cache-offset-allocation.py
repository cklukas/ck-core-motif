#!/usr/bin/env python3
"""Ensure icon-directory caches allocate their terminal name offset."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("MakeCachedDirEntry")
end = source.index("\nstatic String", start)
region = source[start:end]

assert "offset_count = (size_t)numFiles + 1" in region
assert "offset_bytes = offset_count * sizeof(validDir->nameOffsets[0])" in region
assert "XtMalloc(sizeof(DtValidCachedDirStruct) +\n\t\t       offset_bytes +" in region
print("PASS icon cache allocates the terminal name offset")
