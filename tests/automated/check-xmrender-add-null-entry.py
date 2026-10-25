#!/usr/bin/env python3
"""Check render-table addition rejects arrays containing null entries."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableAddRenditions(")
end = source.index("\n/* Remove matching renditions.", start)
region = source[start:end]

assert "for (i = 0; i < rendition_count; i++)" in region
guard = "if (renditions[i] == NULL)"
assert guard in region
assert region.index(guard) < region.index("#ifdef XTHREADS")
print("PASS XmRenderTableAddRenditions rejects null array entries")
