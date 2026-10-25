#!/usr/bin/env python3
"""Check that legacy heap allocation rounds safely before capacity checks."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
start = source.index("_XmHeapAlloc(")
end = source.index("void \n_XmHeapFree", start)
region = source[start:end]

assert region.index("bytes > UINT_MAX - 7") < region.index("heap->bytes_remaining < bytes")
assert region.index("bytes > UINT_MAX - 3") < region.index("heap->bytes_remaining < bytes")
assert region.count("bytes = (bytes +") == 2
assert "bytes_remaining -= bytes" in region
free_start = source.index("_XmHeapFree(")
free_end = source.index("/****************************************************************", free_start)
free_region = source[free_start:free_end]
assert "if (heap == NULL)" in free_region
print("PASS legacy heap alignment overflow is guarded")
