#!/usr/bin/env python3
"""Check serialized widget-tree child storage and index strings are bounded."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrmCreateWidgetTree (")
end = text.find("\n\n\f", start)
region = text[start:end]
required = (
    "widgetrec->size - widgetrec->children_offs <",
    "sizeof(RGMChildrenDesc) - sizeof(RGMChildDesc)",
    "childrendesc->count < 0 || childrendesc->count > RGMListSizeMax",
    "UrmFreeResourceContext (child_ctx)",
    "childptr->key.index_offs < RGMWidgetRecordHdrSize",
    "childptr->key.index_offs >= widgetrec->size",
    "memchr ((char *)widgetrec + childptr->key.index_offs",
)
for needle in required:
    if needle not in region:
        raise SystemExit(f"missing widget-tree bound: {needle}")
print("PASS Mrm widget-tree child descriptors and index strings are bounded")
