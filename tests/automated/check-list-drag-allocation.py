#!/usr/bin/env python3
"""Check list drag initiation handles temporary allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "List.c"
text = source.read_text(encoding="utf-8")
start = text.index("ListDragConv = (XmListDragConvertStruct *)")
end = text.index("\n  /* OK, now start the drag... */", start)
region = text[start:end]
assert "if (ListDragConv == NULL)" in region
assert region.count("if (ListDragConv->strings == NULL)") == 2
assert "lw->list.drag_conv = NULL;" in region
print("list drag initiation guards conversion allocations")
