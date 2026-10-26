#!/usr/bin/env python3
"""Check I18List public helpers reject null widgets before list access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)

functions = (
    ("XmI18ListFindRow(Widget w, String str, int *found_column,", "return NULL;"),
    ("Xm18IListUnselectAllItems( Widget w )", "return;"),
    ("Xm18IListUnselectItem( Widget w, XmMultiListRowInfo *row_info )", "return;"),
    ("XmI18ListToggleRow(Widget w, short row)", "return;"),
)
for signature, result in functions:
    start = source.rindex(signature)
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (w == (Widget) NULL)\n"
    assert guard in body
    assert result in body
    access = "XmI18ListWidget ilist = (XmI18ListWidget) w;"
    if access in body:
        assert body.index(guard) < body.index(access)
    else:
        assert body.index(guard) < body.index("ToggleRow(w, row)")

print("I18List public helpers guard null widgets before list access")
