#!/usr/bin/env python3
"""Check every FontSelector child-argument merge is bounded."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text(encoding="utf-8")

helper_start = source.index("FontSelectorMergeArgLists(ArgList local_args")
helper_end = source.index("CreateChildren(XmFontSelectorWidget", helper_start)
helper = source[helper_start:helper_end]
assert "num_local_args > (Cardinal)(~(Cardinal)0) - num_caller_args" in helper
assert "return XtMergeArgLists(local_args, num_local_args," in helper

calls = source.count("margs = FontSelectorMergeArgLists(")
assert calls == 23
assert source.count("XtMergeArgLists(") == 1

offset = 0
for _ in range(calls):
    start = source.index("margs = FontSelectorMergeArgLists(", offset)
    following = source[start:start + 240]
    assert "if (margs == NULL)" in following
    offset = start + 1

print(f"FontSelector argument merges are bounded ({calls} call sites)")
