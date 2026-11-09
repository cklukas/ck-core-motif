#!/usr/bin/env python3
"""Keep directory-entry list growth within Xt's Cardinal allocation range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(
    encoding="utf-8"
)
cache_start = source.index("AddEntryToCache(char *entryName,")
cache_end = source.index("static String\nAllocatePathBuffer", cache_start)
cache_body = source[cache_start:cache_end]
assert "(Cardinal)((size_t)new_alloc * sizeof(*new_cache))" in cache_body

start = source.index("if (*pNumEntries == *pNumAlloc)")
end = source.index("if (dirLen >= UINT_MAX", start)
body = source[start:end]

increment = body.index("new_alloc = *pNumAlloc + FILE_LIST_BLOCK;")
size_guard = body.index("(size_t)new_alloc > SIZE_MAX / sizeof(*new_entries)")
cardinal_guard = body.index(
    "(size_t)new_alloc > (size_t)UINT_MAX / sizeof(*new_entries)"
)
realloc = body.index("new_entries = (String *)XtRealloc(")
byte_cast = body.index("(Cardinal)((size_t)new_alloc * sizeof(*new_entries))", realloc)

assert increment < size_guard < cardinal_guard < realloc < byte_cast

print("PASS directory-entry list growth validates Xt allocation bytes")
