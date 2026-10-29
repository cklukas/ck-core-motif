#!/usr/bin/env python3
"""Check selected List helpers reject non-List widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)

guards = {
    "XmListGetSelectedPos": "if (w == NULL || !XmIsList(w))",
    "XmListSetHorizPos": "if (w == NULL || !XmIsList(w))",
    "XmListYToPos": "if (w == NULL || !XmIsList(w))",
    "XmListUpdateSelectedList": "if (w == NULL || !XmIsList(w))",
    "XmListPosSelected": "if (w == NULL || !XmIsList(w))",
    "XmListSetAddMode": "if (w == NULL || !XmIsList(w))",
    "XmListItemExists": "if (w == NULL || !XmIsList(w) || item == NULL)",
    "XmListItemPos": "if (w == NULL || !XmIsList(w) || item == NULL)",
    "XmListGetKbdItemPos": "if (w == NULL || !XmIsList(w))",
    "XmListSetKbdItemPos": "if (w == NULL || !XmIsList(w))",
    "XmListGetMatchPos": "if (w == NULL || !XmIsList(w) || item == NULL)",
}
for name, guard in guards.items():
    start = source.index("\n" + name + "(") + 1
    body = source[start : source.index("\n}\n", start) + 3]
    if guard not in body:
        raise SystemExit(f"{name} lacks List class validation")
    if body.index(guard) > body.index("_XmWidgetToAppContext(w)"):
        raise SystemExit(f"{name} validates its widget after context lookup")

print("List selection and geometry helpers validate widget class before access")
