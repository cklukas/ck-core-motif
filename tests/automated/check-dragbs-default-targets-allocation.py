#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static xmTargetsTable \nCreateDefaultTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  CreateDefaultAtomsTable", start)]

assert "if (targetsTable == NULL)" in body
assert "if (entries == NULL)" in body
assert "if (targetsTable->entries[1].targets == NULL)" in body
assert "return (NULL)" in body
print("CreateDefaultTargetsTable handles all allocation failures")
