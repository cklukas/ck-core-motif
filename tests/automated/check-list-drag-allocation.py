#!/usr/bin/env python3
"""Check list drag initiation handles temporary allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "List.c"
text = source.read_text(encoding="utf-8")
start = text.index("ListDragConv = (XmListDragConvertStruct *)")
end = text.index("\n  /* OK, now start the drag... */", start)
region = text[start:end]
assert "SIZE_MAX / sizeof(XmString)" in region
assert "if (ListDragConv == NULL)" in region
assert region.count("if (ListDragConv->strings == NULL)") == 2
assert "lw->list.drag_conv = NULL;" in region
assert "if (ListDragConv->strings[i] == NULL)" in region
assert "while (i != 0)" in region
cleanup_start = text.index("DragDropFinished(Widget w,", text.index("/*ARGSUSED*/", text.index("ListCopyToClipboard")))
cleanup_end = text.index("ListProcessDrag(Widget wid,", cleanup_start)
assert "lw->list.drag_conv = NULL;" in text[cleanup_start:cleanup_end]
print("list drag initiation guards conversion allocations")
