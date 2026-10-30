#!/usr/bin/env python3
"""Check ComboBox edit-field allocation failure guards."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()

children_start = source.index("CreateChildren(Widget")
children = source[children_start:source.index("/*\n * CreatePulldown()", children_start)]
assert "if (CB_EditBox(cb) == NULL)" in children
assert children.index("if (CB_EditBox(cb) == NULL)") < children.index(
    "XtAddCallback(CB_EditBox(cb)")

edit_start = source.index("CreateEditBox(Widget")
edit = source[edit_start:source.index("/*\n * CreatePulldown()", edit_start)]
assert "if (text_widget == NULL)" in edit
assert edit.index("if (text_widget == NULL)") < edit.index("XtManageChild(text_widget)")

print("ComboBox edit-field allocation failures are guarded")
