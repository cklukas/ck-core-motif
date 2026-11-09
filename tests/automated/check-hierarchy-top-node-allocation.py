#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Hierarchy.c").read_text()
start = source.index("static void \nInitialize(")
region = source[start:]
assert "top_node = ((HierarchyConstraints)" in region
assert "if (top_node == NULL)" in region
assert region.index("if (top_node == NULL)") < region.index("XmHierarchyC_state(top_node)")
assert 'XtError("Unable to allocate Hierarchy top-node constraint");' in region
print("PASS Hierarchy top-node allocation failure is explicit and terminal")
