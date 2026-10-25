#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Tree.c").read_text()
start = source.index("static void \nInitialize(Widget req")
end = source.index("/*\tFunction Name: Realize", start)
region = source[start:end]
assert "XtRealloc((XtPointer) XmHierarchy_top_node(tw)" in region
assert "if (top_node == NULL)" in region
assert "XmTree_child_op_list(tw) = _XmListInit();" in region
assert "if (XmTree_child_op_list(tw) == NULL)" in region
print("Tree initialization allocation guards present")
