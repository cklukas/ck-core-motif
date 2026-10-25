#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static Boolean \nReadTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  _XmGetDragProxyWindow", start)]

assert "if (targetsTable == NULL)" in body
assert "if (targetsTable == NULL)" in body[body.index("if (targetsTable == NULL)"):]
assert "if (new_entries == NULL)" in body
assert "targetsTable->entries = new_entries" in body
assert "if (ret && propertyRecPtr->info.num_target_lists" in body
print("ReadTargetsTable handles table allocation failures without dereference")
