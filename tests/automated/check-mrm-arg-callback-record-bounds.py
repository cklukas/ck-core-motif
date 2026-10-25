#!/usr/bin/env python3
"""Check callback-valued Mrm arguments before legacy/current translation."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "case MrmRtypeCallback:",
    "UrmCallbackRecordValid(widgetrec,",
    "argptr->arg_val.datum.offset,",
    "Boolean old_callbacks",
):
    if needle not in region:
        raise SystemExit(f"missing callback-argument bound: {needle}")
print("Mrm callback-valued arguments are record-bounded")
