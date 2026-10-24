#!/usr/bin/env python3
"""Check Mrm data-entry insertion validates record free-space metadata."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__DB_PutDataEntry (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "dataheader->free_ptr > IDBDataFreeMax - dataheader->free_count"
if region.count(needle) < 2:
    raise SystemExit("Mrm data-entry insertion lacks free-space metadata checks")
if region.index(needle) > region.index("simpledata ="):
    raise SystemExit("Mrm insertion forms its destination before validating capacity")
print("Mrm data-entry insertion validates free-space metadata before pointer formation")
