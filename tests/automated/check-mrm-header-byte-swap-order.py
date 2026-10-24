#!/usr/bin/env python3
"""Check Mrm header readers swap entry fields before size validation."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
for name in ("Idb__HDR_GetDataEntry", "Idb__HDR_MatchFilter"):
    start = text.index("\n" + name + " (")
    end = text.find("\n", start + 1)
    region = text[start:] if end < 0 else text[start:end]
    swap = region.index("SwapIDBDataEntryHdr")
    size = region.index("datahdr->entry_size >")
    if swap > size:
        raise SystemExit(f"{name} validates size before byte swapping")
print("Mrm header readers swap entry fields before size validation")
