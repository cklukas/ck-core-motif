#!/usr/bin/env python3
"""Check overflow guards around the Motif drag atom property tables."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c"
text = source.read_text()
for needle in (
    "atomsTable->numEntries > UINT16_MAX",
    "(size_t)UINT_MAX / sizeof(xmMotifAtomsTableRec)",
    "dataSize > UINT32_MAX || dataSize > (size_t)INT_MAX",
    "for (i = 0; ret && i < (int)propertyRecPtr->info.num_atoms; i++)",
):
    assert needle in text, needle
print("drag atom allocation bounds contract passed")
