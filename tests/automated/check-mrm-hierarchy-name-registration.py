#!/usr/bin/env python3
"""Check Mrm hierarchy name registration handles allocation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__RegisterNamesInHierarchy (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "names == NULL || values == NULL",
    "if (name_table == NULL)",
    "if (hash_entry == NULL)",
):
    if needle not in region:
        raise SystemExit(f"Mrm hierarchy name-registration guard is missing: {needle}")

print("Mrm hierarchy name registration handles allocation failures")
