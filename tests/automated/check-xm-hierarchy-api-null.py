#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Hierarchy.c").read_text()
assert text.count("if (nw == NULL)") >= 2
for name in ("XmHierarchyOpenAllAncestors", "XmHierarchyGetChildNodes"):
    assert name in text
print("XmHierarchy APIs guard null nodes")
