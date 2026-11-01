#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ColorS.c").read_text(encoding="utf-8")

helper_start = source.index("ColorSelectorMergeArgLists(ArgList local_args")
helper_end = source.index("static void\nread_rgb_file", helper_start)
helper = source[helper_start:helper_end]
assert "num_local_args > (Cardinal)(~(Cardinal)0) - num_supplied_args" in helper
assert "*merged_num_args = num_local_args + num_supplied_args;" in helper
assert "return XtMergeArgLists(local_args, num_local_args," in helper

for signature, next_signature, expected_merges in (
    ("read_rgb_file(XmColorSelectorWidget csw", "find_name(char *buffer)", 1),
    ("CreateColorSliders(XmColorSelectorWidget csw", "CreateSelectorRadio(XmColorSelectorWidget csw", 2),
    ("CreateSelectorRadio(XmColorSelectorWidget csw", "CreateColorWindow(XmColorSelectorWidget csw", 2),
    ("CreateColorWindow(XmColorSelectorWidget csw", "GetValues_XmNredSliderLabel ( Widget", 1),
):
    start = source.index(signature)
    end = source.index(next_signature, start)
    body = source[start:end]
    assert body.count("ColorSelectorMergeArgLists") == expected_merges
    assert body.count("if (margs == NULL)") >= expected_merges
    assert "merged_num_args" in body

assert source.count("XtMergeArgLists(") == 1
print("ColorSelector argument merges reject overflow and allocation failure")
