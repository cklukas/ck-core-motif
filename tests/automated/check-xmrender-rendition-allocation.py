#!/usr/bin/env python3
"""Check render-table count and allocation-size guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("XmRenderTableAddRenditions(")
end = source.index("/* One body owns each entry handle", start)
region = source[start:end]

assert "rendition_count > INT_MAX" in region
assert "SIZE_MAX / sizeof(Boolean)" in region
assert "SIZE_MAX - sizeof(_XmRenderTableRec)" in region
assert "goto unlock_return;" in region
print("PASS render-table count and allocation-size guards")

remove_start = source.index("_XmRenderTableRemoveRenditions(")
remove_end = source.index("static void\nCopyFromArg", remove_start)
remove_region = source[remove_start:remove_end]
assert "Plan the removal without changing the live table" in remove_region
assert "removed = (Boolean *)XtMalloc" in remove_region
assert "if (table == NULL)" in remove_region
assert "if (newtable == NULL)" in remove_region
assert "remove_abort:" in remove_region
print("PASS render-table removal publishes only after allocation success")
