#!/usr/bin/env python3
"""Check ListConvert validates callback inputs before widget-dependent work."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.index("ListConvert(Widget w")
body = source[start:source.index("\n  assert(XtNumber(atom_names)", start)]

assert "Atom C_ENCODING;" in body
assert "XmListWidget lw;" in body
assert "XmListDragConvertStruct *ListDragConv;" in body
assert "if (w == NULL || cs == NULL)" in body
assert "C_ENCODING = XmeGetEncodingAtom(w);" in body
assert "ListDragConv = lw->list.drag_conv;" in body

print("XmList conversion validates callback inputs")
