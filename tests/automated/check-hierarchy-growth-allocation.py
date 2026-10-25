#!/usr/bin/env python3
"""Check Hierarchy list growth keeps live storage on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hierarchy.c").read_text()
build = source[source.index("\nBuildNodeTable(Widget w"):source.index("\nResetOpenCloseButton(Widget w")]
children = source[source.index("\nAddChildToList(HierarchyConstraints"):source.index("\nGetNodeInfo")]
assert "HierarchyConstraints *new_table" in build
assert "if (new_table == NULL)" in build
assert "XmHierarchy_node_table(hw) = new_table" in build
assert "HierarchyConstraints *new_list" in children
assert "if (new_list == NULL)" in children
assert "*list = new_list" in children
assert "SIZE_MAX / node_count" in build
assert "SIZE_MAX / list_count" in children
assert "#include <Xm/Ext.h>" in source
print("PASS Hierarchy growth preserves lists on allocation failure")
