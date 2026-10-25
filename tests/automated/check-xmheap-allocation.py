#!/usr/bin/env python3
"""Check that the legacy XmHeap constructor handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
start = source.index("_XmHeapCreate(")
end = source.index("char * \n_XmHeapAlloc", start)
region = source[start:end]
assert "heap = XtNew(XmHeapRec);" in region
assert "if (heap == NULL)" in region
assert "return NULL;" in region
assert "bytes > UINT_MAX - sizeof(char*)" in source
assert "if (heap_loc == NULL)" in source
assert "heap->segment_size <= (int)sizeof(char*)" in source
print("PASS XmHeap allocation is guarded")
