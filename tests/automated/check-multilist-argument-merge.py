#!/usr/bin/env python3
"""Check MultiList child argument-list merges are bounded and null-safe."""

from pathlib import Path


source = Path("lib/Xm/MultiList.c").read_text(encoding="utf-8")

helper_start = source.index("MultiListMergeArgLists(ArgList args")
helper_end = source.index("static Dimension\nmultilist_frame_dimension", helper_start)
helper = source[helper_start:helper_end]
assert "num_args > (Cardinal)(~(Cardinal)0) - num_local_args" in helper
assert "*merged_num_args = num_args + num_local_args;" in helper
assert "return XtMergeArgLists(args, num_args, local_args, num_local_args);" in helper

for function, next_function in (
    ("CreateTitle", "CreateScrollbar"),
    ("CreateScrollbar", "CreateFindButton"),
    ("CreateFindButton", "CreateFindText"),
):
    start = source.index(f"{function}(Widget parent")
    end = source.index(f"{next_function}(", start)
    body = source[start:end]
    assert "MultiListMergeArgLists" in body
    assert "if (new_args == NULL" in body
    assert "merged_num_args" in body

frame_start = source.index("CreateFrame(Widget parent")
frame_end = source.index("MultiListMergeArgLists(ArgList args", frame_start)
frame = source[frame_start:frame_end]
assert frame.count("MultiListMergeArgLists") == 2
assert "if (new_args == NULL)\n\treturn(w);" in frame
assert "XmMultiList_ilist(elist) = XtCreateManagedWidget" in frame

print("PASS MultiList argument merges reject overflow and allocation failure")
