#!/usr/bin/env python3
"""Check Mrm hierarchy name APIs reject invalid handles and null names."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
reg_start = text.index("Urm__RegisterNamesInHierarchy (MrmHierarchy")
reg_end = text.index("Urm__LookupNameInHierarchy (MrmHierarchy", reg_start)
reg = text[reg_start:reg_end]
lookup = text[reg_end:]
for needle in (
    "hierarchy_id == NULL || !MrmHierarchyValid(hierarchy_id)",
    "names[ndx] == NULL",
):
    if needle not in reg:
        raise SystemExit(f"missing hierarchy registration guard: {needle}")
if "name == NULL || value_return == NULL" not in lookup:
    raise SystemExit("missing hierarchy name lookup guard")
print("PASS Mrm hierarchy name APIs reject invalid handles and null names")
