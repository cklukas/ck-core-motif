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

success_cleanup = block.index("while(--atom_count >= 0)")
success_vector_free = block.index("XtFree((char *)name_list);", success_cleanup)
output_handoff = block.index("static Atom *buf;", success_cleanup)
if not success_vector_free < output_handoff:
    raise SystemExit("atom-list converter leaks a grown name vector after XInternAtoms")

print("PASS atom-list conversion releases grown token storage on success and failure")
