#!/usr/bin/env python3
"""Check that Mrm data-entry readers bound the record offset before dereference."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
needle = "data_entry.item_offs > IDBDataFreeMax - sizeof(IDBDataEntryHdr)"
if text.count(needle) < 2:
    raise SystemExit("Mrm data-entry readers do not both validate item offsets")
for name in ("Idb__DB_GetDataEntry", "Idb__DB_MatchFilter"):
    start = text.index("\n" + name + " (")
    end = text.find("\n", start + 1)
    region = text[start:] if end < 0 else text[start:end]
    check = region.index(needle)
    deref = region.index("data_rec->data[data_entry.item_offs]")
    if check > deref:
        raise SystemExit(f"{name} checks the offset after dereferencing it")
print("Mrm data-entry readers validate item offsets before record access")
