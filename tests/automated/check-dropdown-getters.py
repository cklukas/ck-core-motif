#!/usr/bin/env python3
"""Check DropDown public child getters preserve the actual child identities."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()

list_start = source.index("Widget XmDropDownGetList(Widget w)")
list_body = source[list_start:source.index("Widget XmDropDownGetChild(Widget w, int num)", list_start)]
assert "return XmDropDown_list(w);" in list_body
assert "XtNameToWidget(w, \"*list\")" not in list_body

for getter, child in (
    ("XmDropDownGetLabel", "XmDropDown_label"),
    ("XmDropDownGetArrow", "XmDropDown_arrow"),
    ("XmDropDownGetText", "XmDropDown_text"),
    ("XmDropDownGetList", "XmDropDown_list"),
):
    start = source.index(f"Widget {getter}(Widget w)")
    body = source[start:source.index("\n}\n", start) + 3]
    assert "w == NULL || !XtIsSubclass(w, xmDropDownWidgetClass)" in body
    assert f"return {child}(w);" in body

print("DropDown child getters return validated internal child identities")
