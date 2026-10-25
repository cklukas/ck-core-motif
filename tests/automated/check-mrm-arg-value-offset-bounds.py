#!/usr/bin/env python3
"""Check non-immediate Mrm argument offsets before pointer evaluation."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "reptype != MrmRtypeInteger",
    "reptype != MrmRtypeSingleFloat",
    "reptype != MrmRtypeNull",
    "argptr->arg_val.datum.offset < RGMWidgetRecordHdrSize",
    "argptr->arg_val.datum.offset >= widgetrec->size",
):
    if needle not in region:
        raise SystemExit(f"missing argument offset bound: {needle}")
print("Mrm non-immediate argument offsets are record-bounded")
