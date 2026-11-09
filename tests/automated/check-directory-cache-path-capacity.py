#!/usr/bin/env python3
"""Keep directory-cache stat paths sized for every directory-entry name."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(
    encoding="utf-8"
)
assert "static size_t dirCacheNameAlloc;" in source

start = source.index("AddEntryToCache(char *entryName,")
end = source.index("static String\nAllocatePathBuffer", start)
body = source[start:end]

size = body.index("full_name_size = (size_t)dirCacheNameLen + entryNameLen + 1;")
grow = body.index("if (full_name_size > dirCacheNameAlloc)", size)
realloc = body.index("new_name = XtRealloc(dirCacheName, (Cardinal)full_name_size);", grow)
copy = body.index("strcpy(&dirCacheName[dirCacheNameLen], entryName);", realloc)

assert size < grow < realloc < copy
assert "SIZE_MAX - entryNameLen - 1" in body
assert "UINT_MAX - entryNameLen - 1" in body

print("PASS directory cache grows its stat path for long directory entries")
