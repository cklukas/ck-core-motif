#!/usr/bin/env python3
"""Ensure XmOutline never continues with a missing child-operation list."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Outline.c").read_text(
    encoding="utf-8"
)
initialize = source[source.index("static void \nInitialize(Widget req, Widget set"):
                    source.index("static void CreateGC", source.index("static void \nInitialize(Widget req, Widget set"))]
assert "XmOutline_child_op_list(ow) = _XmListInit();" in initialize
assert "if (XmOutline_child_op_list(ow) == NULL)" in initialize
assert 'XtError("Unable to allocate Outline child-operation list");' in initialize

layout_start = source.index("static void\nLayoutChildren(")
layout_end = source.index("Unmap all nodes that no longer are visible.", layout_start)
layout = source[layout_start:layout_end]
replacement = layout.index("replacement_child_op_list = _XmListInit();")
failure = layout.index("if (replacement_child_op_list == NULL)", replacement)
release = layout.index("_XmListFree(XmOutline_child_op_list(ow));", replacement)
assign = layout.index("XmOutline_child_op_list(ow) = replacement_child_op_list;", replacement)
assert failure < release < assign
assert 'XtError("Unable to allocate Outline child-operation list");' in layout[failure:release]
assert "XmDropSiteEndUpdate(w);" in layout[failure:release]

print("PASS XmOutline child-operation list failures are explicit and terminal")
