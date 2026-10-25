#!/usr/bin/env python3
"""Check render-table tag extraction rejects malformed entries transactionally."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableGetTags(")
end = source.index("\n\n/* Returns copy of matching rendition.", start)
region = source[start:end]

guard = "_XmRTRenditions(table)[i] == NULL"
assert guard in region
assert "_XmRendTag(_XmRTRenditions(table)[i]) == NULL" in region
assert "tags = NULL" in region
print("PASS XmRenderTableGetTags rolls back malformed entries")
