#!/usr/bin/env python3
"""Check render-table removal validates table, count, and tag entries."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
public_start = source.index("XmRenderTableRemoveRenditions(")
public_end = source.index("\n/* Remove matching renditions.", public_start)
public = source[public_start:public_end]
assert "oldtable == NULL || tags == NULL || tag_count <= 0" in public
assert "if (tags[i] == NULL)" in public
assert public.index("oldtable == NULL") < public.index("#ifdef XTHREADS")

internal_start = source.index("_XmRenderTableRemoveRenditions(XmRenderTable")
internal_end = source.index("\n\n/*", internal_start)
internal = source[internal_start:internal_end]
assert "tag_count <= 0" in internal
assert "if (tags[j] == NULL)" in internal
print("PASS render-table removal validates null and non-positive inputs")
