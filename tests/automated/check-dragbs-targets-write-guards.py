#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static void \nWriteTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  CreateDefaultTargetsTable", start)]

assert "targetsTable->numEntries > UINT16_MAX" in body
assert "targetsTable->entries == NULL" in body
assert "dataSize > SIZE_MAX - 2" in body
assert "propertyRecPtr == NULL" in body
print("WriteTargetsTable validates serialized size, pointers, and allocation")
