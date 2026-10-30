#!/usr/bin/env python3
"""Check ComboBox popup and list allocation failure guards."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()

children_start = source.index("CreateChildren(Widget")
children = source[children_start:source.index("/*\n * CreatePulldown()", children_start)]
assert "if (CB_ListShell(cb))" in children
assert "if (CB_List(cb))" in children

list_start = source.index("CreateScrolledList(Widget")
list_body = source[list_start:source.index("/*\n * Hit()", list_start)]
assert "if (list == NULL)" in list_body
assert list_body.index("if (list == NULL)") < list_body.index("XtManageChild(list)")
assert list_body.index("if (list == NULL)") < list_body.index("CB_ScrolledW(cb)")

print("ComboBox popup and list allocation failures are guarded")
