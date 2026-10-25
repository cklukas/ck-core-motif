#!/usr/bin/env python3
"""Check RowColumn post-from-list preparation is transactional and bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text()
start = source.rindex("static void \nPreparePostFromList(")
end = source.index("static void", start + len("static void"))
region = source[start:end]
assert "postFromCount > INT_MAX - 1" in region
assert "SIZE_MAX / sizeof(Widget)" in region
assert "UINT_MAX / sizeof(Widget)" in region
assert "newList = (Widget *)XtMalloc((Cardinal)(sizeof(Widget) *" in region
assert "if (newList == NULL)" in region
assert region.index("if (newList == NULL)") < region.index(
    "rowcol->row_column.postFromList = newList")
assert "XtFree((char *)tempPtr);" in region
print("RowColumn post-from-list preparation is transactional")
