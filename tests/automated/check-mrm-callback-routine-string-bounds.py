#!/usr/bin/env python3
"""Check Mrm callback routine offsets before string lookup."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_FixupCallback (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "itmptr->cb_item.routine < RGMWidgetRecordHdrSize",
    "itmptr->cb_item.routine >= ((RGMWidgetRecordPtr)bufptr)->size",
    "memchr ((char *)bufptr + itmptr->cb_item.routine",
):
    if needle not in region:
        raise SystemExit(f"missing callback routine bound: {needle}")
print("Mrm callback routine names are record-bounded")
