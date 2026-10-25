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
