#!/usr/bin/env python3
"""Check serialized Mrm argument-list descriptors are fully bounded."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
required = (
    "sizeof(RGMArgListDesc) - sizeof(RGMArgument)",
    "argdesc->count < 0 || argdesc->extra < 0",
    "argdesc->count > RGMListSizeMax ||",
    "argdesc->extra > RGMListSizeMax ||",
    "sizeof(RGMArgument))",
)
for needle in required:
    if text.count(needle) < 2:
        raise SystemExit(f"missing argument-list bound in both instance paths: {needle}")
print("PASS Mrm serialized argument-list descriptors are bounded")
