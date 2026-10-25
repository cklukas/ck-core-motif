#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static void \nWriteAtomsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  ReadAtomsTable", start)]

assert "atomsTable->numEntries > UINT16_MAX" in body
assert "propertyRecPtr->info.num_atoms = atomsTable->numEntries" in body
print("WriteAtomsTable rejects entry counts that do not fit the wire field")
