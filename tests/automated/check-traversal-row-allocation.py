#!/usr/bin/env python3
"""Check traversal row storage growth handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TraversalI.c").read_text()
append_start = source.index("AppendToRow(XmTraversalNode item")
append = source[append_start:source.index("\nstatic Boolean\nNodeDominates", append_start)]
sort_start = source.index("Sort(XmTraversalNode *list")
sort = source[sort_start:source.index("\nstatic int \nCompareExclusive", sort_start)]
assert "static Boolean\nAppendToRow" in source
assert "XmTraversalNode *new_items" in append
assert "if (new_items == NULL)" in append
assert "return False;" in append
assert "return True;" in append
assert "new_data.items = (XmTraversalNode *)XtMalloc" in sort
assert "if (new_data.items == NULL)" in sort
assert "XmTraversalRow *new_rows" in sort
assert "if (new_rows == NULL)" in sort
assert "new_row_count > UINT_MAX / sizeof(*new_rows)" in sort
assert "(Cardinal)(new_row_count * sizeof(*new_rows))" in sort
assert "allocation_failure:" in sort
print("PASS traversal row growth handles allocation failure")
