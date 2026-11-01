#!/usr/bin/env python3
"""Check DropDown child argument-list merges are bounded and null-safe."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text(encoding="utf-8")

helper_start = source.index("DropDownMergeArgLists(ArgList args")
helper_end = source.index("static void\nPopupDestroyed", helper_start)
helper = source[helper_start:helper_end]
assert "num_args > (Cardinal)(~(Cardinal)0) - num_local_args" in helper
assert "*merged_num_args = num_args + num_local_args;" in helper
assert "return XtMergeArgLists(args, num_args, local_args, num_local_args);" in helper

children_start = source.index("CreateChildren(Widget w")
children_end = source.index("SBBtnDownEH", children_start)
children = source[children_start:children_end]
assert children.count("DropDownMergeArgLists") == 3
assert children.count("if (merge == NULL)") == 3
assert "w, merge, merged_num_args" in children
assert "num_args+tn" not in children
assert "num_args + tn" not in children

popup_start = source.index("CreatePopup(Widget w")
popup_end = source.index("DropDownMergeArgLists(ArgList args", popup_start)
popup = source[popup_start:popup_end]
assert popup.count("DropDownMergeArgLists") == 2
assert popup.count("if (new_list == NULL)") == 2
assert "XtDestroyWidget(XmDropDown_popup_shell(cbw));" in popup
assert "merged_num_args" in popup
assert "num_largs + num_args" not in popup

print("PASS DropDown argument merges reject overflow and allocation failure")
