#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static Boolean \nReadAtomsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  WriteTargetsTable", start)]

assert "type == atomsTableAtom" in body
assert "format == 8" in body
assert "lengthRtn < sizeof(xmMotifAtomsPropertyRec)" in body
assert "sizeof(xmMotifAtomsTableRec)" in body
assert "if (ret && atomsTable == NULL)" in body
print("ReadAtomsTable validates property encoding and complete entry storage")
