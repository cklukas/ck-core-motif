#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ComboBox.c").read_text(encoding="utf-8")

helper_start = source.index("ComboBoxMergeArgLists(ArgList caller_args")
helper_end = source.index("/*\n * CreateEditBox()", helper_start)
helper = source[helper_start:helper_end]
assert "num_caller_args > (Cardinal)(~(Cardinal)0) - num_local_args" in helper
assert "*merged_num_args = num_caller_args + num_local_args;" in helper
assert "return XtMergeArgLists(caller_args, num_caller_args," in helper

for signature, next_signature, expected_merges in (
    ("CreateEditBox(Widget    parent", "CreatePulldown(Widget", 1),
    ("CreatePulldown(Widget", "CreateScrolledList(Widget", 1),
    ("CreateScrolledList(Widget", "Hit(XButtonEvent", 1),
    ("XmCreateDropDownComboBox(Widget", "XmCreateDropDownList(Widget", 1),
    ("XmCreateDropDownList(Widget", "XmVaCreateComboBox(", 1),
):
    start = source.index(signature)
    end = source.index(next_signature, start)
    body = source[start:end]
    assert body.count("ComboBoxMergeArgLists") == expected_merges
    assert "if (merged_args == NULL)" in body
    assert "merged_num_args" in body

edit_start = source.index("CreateEditBox(Widget    parent")
edit_end = source.index("CreatePulldown(Widget", edit_start)
edit = source[edit_start:edit_end]
assert edit.index("if (text_widget == NULL)") < edit.index("_XmAppearanceInheritFont")
assert edit.index("if (item)\n      XtFree(item);") < edit.index("return NULL;", edit.index("if (merged_args == NULL)"))
assert source.count("XtMergeArgLists(") == 1
print("ComboBox argument merges reject overflow and allocation failure")
