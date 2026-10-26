#!/usr/bin/env python3
"""Ensure XmTree does not continue after child-list initialization failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Tree.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nInitialize(Widget req, Widget set")
end = source.index("\n/*\tFunction Name: Realize", start)
region = source[start:end]

assert "XmTree_child_op_list(tw) = _XmListInit();" in region
assert "if (XmTree_child_op_list(tw) == NULL) {" in region
assert 'XtError("Unable to allocate Tree child-operation list");' in region
assert "return;" in region

print("XmTree initialization: child-list failure is explicit and terminal")
