#!/usr/bin/env python3
"""Check Mrm hierarchy opening validates required pointer arguments."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__OpenHierarchy (")
end = text.index("\n}\n", start)
region = text[start:end]
guard = region.index("if (num_files < 0")
for needle in (
    "hierarchy_id_return == NULL",
    "(num_files > 0 && name_list == NULL)",
    "(in_memory && uid_buffer == NULL)",
):
    if needle not in region[guard:]:
        raise SystemExit(f"Mrm hierarchy-open guard is missing: {needle}")

print("Mrm hierarchy opening validates required pointers")
