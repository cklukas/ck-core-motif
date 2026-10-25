#!/usr/bin/env python3
"""Check callback datum offsets before callback-value pointer evaluation."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_FixupCallback (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "itmptr->cb_item.datum.offset < RGMWidgetRecordHdrSize",
    "itmptr->cb_item.datum.offset >=",
    "((RGMWidgetRecordPtr)bufptr)->size",
    "reptype != MrmRtypeNull",
):
    if needle not in region:
        raise SystemExit(f"missing callback-value bound: {needle}")
print("Mrm callback datum offsets are record-bounded")
