#!/usr/bin/env python3
"""Check I18List drag snapshots roll back allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("lw->ilist.drag_conv = ListDragConv")
end = source.index("\n    /* OK, now start the drag... */", start)
region = source[start:end]

assert "if (ListDragConv == NULL)" in source
assert "if (ListDragConv->strings == NULL)" in region
assert "if (ListDragConv->strings[i] == NULL)" in region
assert "while (i != 0)" in region
assert "XmStringCopy(lw->ilist.column_titles[col])" in region
cleanup = source[source.index("FreeDragSnapshot(XmI18ListWidget"):]
assert "if (ListDragConv == NULL)" in cleanup
assert "lw->ilist.drag_conv = NULL;" in cleanup
print("I18List drag snapshots handle allocation failure")
