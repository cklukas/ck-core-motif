#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static void \nWriteAtomsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  ReadAtomsTable", start)]

assert "atomsTable->numEntries != 0 && atomsTable->entries == NULL" in body
assert "propertyRecPtr == NULL" in body
assert "return;" in body
print("WriteAtomsTable rejects incomplete tables and allocation failure")
