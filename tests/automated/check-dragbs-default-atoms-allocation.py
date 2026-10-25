#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static xmAtomsTable \nCreateDefaultAtomsTable")
body = text[start:text.index("_XmInitTargetsTable (", start)]

assert "if (atomsTable == NULL)" in body
assert "if (atomsTable->entries == NULL)" in body
assert "XtFree((char *)atomsTable)" in body
assert "return (NULL)" in body
print("CreateDefaultAtomsTable handles table and entry allocation failures")
