#!/usr/bin/env python3
"""Check bounded growth for the Atom-list resource converter."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")
start = text.index("static Boolean\nCvtStringToAtomList(")
end = text.index("\n/*ARGSUSED*/\nstatic Boolean", start)
region = text[start:end]

for needle in (
    "max_atoms > INT_MAX / 2",
    "SIZE_MAX / (2 * sizeof(char *))",
    "UINT_MAX / (2 * sizeof(char *))",
    "SIZE_MAX / sizeof(Atom)",
    "UINT_MAX / sizeof(Atom)",
    "(Cardinal)(sizeof(char *) * (size_t)max_atoms)",
    "(Cardinal)(sizeof(Atom) * (size_t)atom_count)",
    "if (name_list != stack_names)",
):
    if needle not in region:
        raise SystemExit(f"Atom-list capacity guard missing: {needle}")

print("Atom-list allocation sizing is bounded")
