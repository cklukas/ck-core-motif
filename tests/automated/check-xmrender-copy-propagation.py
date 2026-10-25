#!/usr/bin/env python3
"""Check that XmRenderTableCopy bounds its table-body allocation size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
copy_start = source.index("XmRenderTableCopy(")
copy_end = source.index("/* One body owns each entry handle", copy_start)
copy_region = source[copy_start:copy_end]
assert "SIZE_MAX - sizeof(_XmRenderTableRec)" in copy_region
assert "capacity - RENDITIONS_IN_STRUCT" in copy_region
assert "goto copy_unlock_return;" in copy_region
assert "if (body == NULL)" in copy_region
assert "if (result == NULL)" in copy_region
assert "copy_failed:" in copy_region
assert "while (count > 0)" in copy_region
assert "_XmRenderTableFindRendition(table, tags[index]" in copy_region
print("PASS XmRenderTableCopy bounds table-body allocation size")
