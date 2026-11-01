#!/usr/bin/env python3
"""Check selected List helpers reject non-List widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)

functions = ("XmListGetSelectedPos", "XmListSetHorizPos", "XmListYToPos",
             "XmListUpdateSelectedList", "XmListPosSelected", "XmListSetAddMode",
             "XmListItemExists", "XmListItemPos", "XmListGetKbdItemPos",
             "XmListSetKbdItemPos", "XmListGetMatchPos")
for name in functions:
    start = source.index("\n" + name + "(") + 1
    body = source[start : source.index("\n}\n", start) + 3]
    if "!XmIsList(w)" not in body:
        raise SystemExit(f"{name} lacks List class validation")
    if body.index("!XmIsList(w)") > body.index("_XmWidgetToAppContext(w)"):
        raise SystemExit(f"{name} validates its widget after context lookup")

print("List selection and geometry helpers validate widget class before access")
