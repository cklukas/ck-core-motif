#!/usr/bin/env python3
"""Check that cached Xmos directory names include their terminator."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(
    encoding="utf-8"
)
start = source.index("static unsigned char\nAddEntryToCache")
region = source[start:source.index("GetQualifiedDir", start)]
assert "sizeof(XmDirCacheRec) + entryNameLen + 1" in region
assert "(size_t)entryNameLen > (size_t)UINT_MAX - sizeof(XmDirCacheRec) - 1" in region
assert "(size_t)new_alloc > (size_t)UINT_MAX / sizeof(*new_cache)" in region
assert "strcpy(dirCache[numCacheEntries]->file_name, entryName);" in region
print("Xmos directory-cache entries reserve their terminating byte")
