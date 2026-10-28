#!/usr/bin/env python3
"""Check atom-table reader allocations honor Xt's Cardinal limit."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
start = source.index("static Boolean \nReadAtomsTable(")
end = source.index("/*****************************************************************************\n *\n *  WriteTargetsTable", start)
body = source[start:end]

assert "UINT_MAX / sizeof(xmAtomsTableEntryRec)" in body
assert "XtRealloc(" in body
assert "(Cardinal)((size_t)sizeof(xmAtomsTableEntryRec)" in body
assert "if (new_entries == NULL)" in body

print("PASS DragBS atom-table allocations honor Cardinal bounds")
