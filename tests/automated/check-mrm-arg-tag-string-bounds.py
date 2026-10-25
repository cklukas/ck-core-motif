#!/usr/bin/env python3
"""Check unknown Mrm argument tag strings before use."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "argptr->tag_code == UilMrmUnknownCode",
    "UrmWidgetStringValid(widgetrec,",
    "argptr->stg_or_relcode.tag_offs",
):
    if needle not in region:
        raise SystemExit(f"missing tag-string bound: {needle}")
print("Mrm unknown argument tag strings are record-bounded")
