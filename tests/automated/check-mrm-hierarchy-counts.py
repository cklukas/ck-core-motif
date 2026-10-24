#!/usr/bin/env python3
"""Check Mrm hierarchy file-list sizing before allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__OpenHierarchy (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "size_t\t\tlist_size",
    "num_files < 0",
    "(size_t)num_files > SIZE_MAX / sizeof(IDBFile)",
):
    if needle not in region:
        raise SystemExit(f"Mrm hierarchy count guard missing: {needle}")

print("Mrm hierarchy file-list sizing is bounded")
