#!/usr/bin/env python3
"""Check overflow guards around the Motif drag atom property tables."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c"
text = source.read_text()
for needle in (
    "SIZE_MAX - sizeof(xmMotifAtomsPropertyRec)",
    "SIZE_MAX / sizeof(xmAtomsTableEntryRec)",
    "atomsTable->numEntries == (Cardinal)-1",
    "for (i = 0; ret && i < (int)propertyRecPtr->info.num_atoms; i++)",
):
    assert needle in text, needle
print("drag atom allocation bounds contract passed")
