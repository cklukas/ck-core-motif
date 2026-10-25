#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static void \nWriteTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  CreateDefaultTargetsTable", start)]

assert "dataSize > UINT32_MAX" in body
assert "propertyRecPtr->info.heap_offset = dataSize" in body
print("WriteTargetsTable rejects offsets that do not fit the wire field")
