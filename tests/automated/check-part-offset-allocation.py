#!/usr/bin/env python3
"""Check that Motif part-offset table construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c").read_text(encoding="utf-8")
start = source.index("_XiResolveAllPartOffsets(")
end = source.index("void \nXiResolveAllPartOffsets", start)
region = source[start:end]
assert "if (w_class == NULL || offset == NULL)" in region
assert "*offset = NULL;" in region
assert "classcount > UINT_MAX / sizeof(XmOffset)" in region
assert "if (*offset == NULL)" in region
assert "if (*constraint_offset == NULL)" in region
assert region.count("_XmProcessUnlock();") >= 4
print("PASS Motif part-offset allocations are guarded")
