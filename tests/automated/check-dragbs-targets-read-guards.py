#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static Boolean \nReadTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  _XmGetDragProxyWindow", start)]

assert "type == targetsTableAtom" in body
assert "format == 8" in body
assert "remaining < 2" in body
assert "num_targets * 4" in body
assert "if (ret && targetsTable == NULL)" in body
print("ReadTargetsTable validates serialized target-list boundaries")
