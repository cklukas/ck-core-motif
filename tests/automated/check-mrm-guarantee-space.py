#!/usr/bin/env python3
"""Check Mrm record growth before narrowing aligned deltas."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("UrmCWR__GuaranteeSpace (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "full_delta ;",
    "full_delta = _FULLWORD ((size_t)delta)",
    "full_delta > MrmMaxResourceSize",
    "MrmMaxResourceSize - full_delta",
    "delta = (MrmSize)full_delta",
):
    if needle not in region:
        raise SystemExit(f"Mrm GuaranteeSpace guard missing: {needle}")

print("Mrm GuaranteeSpace growth is bounded")
