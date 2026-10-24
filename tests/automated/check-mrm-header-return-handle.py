#!/usr/bin/env python3
"""Check that Mrm header RID return validates stored data handles."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__HDR_ReturnItem (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
for needle in ("internal_id.rec_no <= 0", "IDBHeaderFreeMax - sizeof(IDBDataEntryHdr)",
               "IDBDataFreeMax - sizeof(IDBDataEntryHdr)"):
    if needle not in region:
        raise SystemExit(f"Mrm RID return lacks stored-handle validation: {needle}")
if region.index("internal_id.rec_no <= 0") > region.index("entry_return->rec_no"):
    raise SystemExit("Mrm RID return validates the handle after publishing it")
print("Mrm header RID return validates stored handle bounds before publication")
