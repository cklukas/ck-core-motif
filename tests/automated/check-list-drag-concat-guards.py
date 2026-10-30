#!/usr/bin/env python3
"""Check Motif-drop list conversion handles missing and failed payloads."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.index("ListConcatDragStrings(XmString *items")
end = source.index("\n\f", start)
body = source[start:end]
assert "items == NULL || separator == NULL || count <= 0" in body
assert "items[i] == NULL" in body
assert "XmStringFree(concat);" in body

convert = source[source.index("ListConvert(Widget w"):]
assert "if (ListDragConv == NULL)" in convert
assert "ListConcatDragStrings(ListDragConv->strings" in convert

print("XmList Motif-drop concatenation is null-safe")
