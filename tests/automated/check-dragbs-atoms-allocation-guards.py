#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static Boolean \nReadAtomsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  WriteTargetsTable", start)]

assert "if (atomsTable == NULL)" in body
assert "new_entries = (xmAtomsTableEntry) XtRealloc" in body
assert "if (new_entries == NULL)" in body
assert "atomsTable->entries = new_entries" in body
print("ReadAtomsTable handles lazy table and entry-array allocation failures")
