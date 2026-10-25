#!/usr/bin/env python3
"""Check callback-valued Mrm arguments before descriptor translation."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
required = (
    "case MrmRtypeCallback:",
    "Boolean old_callbacks =",
    "UrmCallbackRecordValid(widgetrec,",
    "argptr->arg_val.datum.offset",
    "if (old_callbacks)",
)
for needle in required:
    if needle not in region:
        raise SystemExit(f"missing callback argument bound: {needle}")
print("PASS Mrm callback-valued arguments are record-bounded")
