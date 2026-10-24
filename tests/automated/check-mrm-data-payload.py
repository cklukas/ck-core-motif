#!/usr/bin/env python3
"""Check that Mrm data readers bound simple and overflow payload copies."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__DB_GetDataEntry (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
checks = [
    "IDBDataFreeMax - data_entry.item_offs - sizeof(IDBDataEntryHdr)",
    "IDBDataFreeMax - sizeof(IDBOverflowData)",
    "ofl_data->segment_size > IDBDataOverflowMax",
    "ofl_data->segment_size > remaining",
    "if (remaining != 0)",
]
for check in checks:
    if check not in region:
        raise SystemExit(f"missing Mrm payload bound: {check}")
print("Mrm data readers bound simple and overflow payload copies")
