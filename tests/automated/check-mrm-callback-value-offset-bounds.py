#!/usr/bin/env python3
"""Check non-immediate Mrm callback values before pointer evaluation."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_FixupCallback (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "reptype != MrmRtypeInteger",
    "reptype != MrmRtypeSingleFloat",
    "reptype != MrmRtypeNull",
    "itmptr->cb_item.datum.offset < RGMWidgetRecordHdrSize",
    "itmptr->cb_item.datum.offset >=",
):
    if needle not in region:
        raise SystemExit(f"missing callback value bound: {needle}")
print("PASS Mrm non-immediate callback values are record-bounded")
