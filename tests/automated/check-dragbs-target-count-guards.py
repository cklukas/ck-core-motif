#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragBS.c"
text = source.read_text()
start = text.index("static void \nWriteTargetsTable")
body = text[start:text.index("/*****************************************************************************\n *\n *  CreateDefaultTargetsTable", start)]

assert "targetsTable->entries[i].numTargets > UINT16_MAX" in body
assert "shortItem.value = targetsTable->entries[i].numTargets" in body
print("WriteTargetsTable rejects per-list counts that do not fit the wire field")
