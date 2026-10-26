#!/usr/bin/env python3
"""Ensure drag target-table allocations fit Xt's Cardinal size."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
start = source.index("static Boolean \nReadTargetsTable(")
body = source[start:source.index("WriteTargetsTable (", start)]
assert "UINT_MAX / sizeof(xmTargetsTableEntryRec)" in body
assert "UINT_MAX / sizeof(Atom)" in body
print("PASS drag target-table allocations honor Cardinal bounds")
