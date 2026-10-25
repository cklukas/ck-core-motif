#!/usr/bin/env python3
"""Check atom-list converter allocation and cleanup guards."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToAtomList(")
end = text.index("\nSimpleDestructor(", start)
block = text[start:end]

for needle in (
    "if (new_names == NULL)",
    "atom_list = (Atom *) XtMalloc",
    "if (atom_list == NULL)",
    "XtFree((char *)name_list);",
):
    if needle not in block:
        raise SystemExit(f"atom-list converter is missing {needle}")

print("atom-list conversion handles allocation failure")
