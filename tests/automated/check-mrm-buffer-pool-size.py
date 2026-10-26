#!/usr/bin/env python3
"""Check that Mrm rejects an invalid externally configured buffer-pool size."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__BM_InitBufferVector (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "idb__buffer_pool_size <= 0"
if needle not in region:
    raise SystemExit("buffer-pool initialization lacks a non-positive-size guard")
if region.index(needle) > region.index("XtMalloc"):
    raise SystemExit("buffer-pool size is checked after allocation")
print("Mrm buffer-pool initialization rejects non-positive sizes before allocation")

if "(size_t)idb__buffer_pool_size > SIZE_MAX / sizeof(IDBRecordBuffer)" not in region:
    raise SystemExit("buffer-pool allocation has no multiplication bound")
if "(size_t)idb__buffer_pool_size > UINT_MAX / sizeof(IDBRecordBuffer)" not in region:
    raise SystemExit("buffer-pool allocation has no Xt width bound")
print("Mrm buffer-pool multiplication is bounded")
