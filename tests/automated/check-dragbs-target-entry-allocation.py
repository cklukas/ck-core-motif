#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static Boolean \nReadTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  _XmGetDragProxyWindow", start)]

assert "old_num_entries = targetsTable->numEntries" in body
assert "num_targets != 0 && targets == NULL" in body
assert "while (targetsTable->numEntries > old_num_entries)" in body
assert "ret = False" in body
print("ReadTargetsTable rolls back partial target entries after allocation failure")
