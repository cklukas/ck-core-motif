#!/usr/bin/env python3
"""Check Mrm widget-tree child descriptor offsets and counts."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrmCreateWidgetTree (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "if ( widgetrec->children_offs >= widgetrec->size )",
    "if ( childrendesc->count > RGMListSizeMax )",
):
    if needle not in region:
        raise SystemExit(f"missing child-tree guard: {needle}")
print("PASS Mrm widget-tree child metadata is bounded")
