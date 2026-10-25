#!/usr/bin/env python3
"""Check Hierarchy child-count paths use full-width Cardinal indices."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hierarchy.c").read_text(
    encoding="utf-8"
)
for signature in (
    "ConstraintSetValues(Widget current",
    "ChangeNodeState(HierarchyConstraints node)",
    "UnmapAllExtraNodes(Widget w, HierarchyConstraints node)",
    "RemoveChildren(HierarchyConstraints * list, Cardinal num)",
    "AddChildToList(HierarchyConstraints ** list, Cardinal * num",
    "RemoveNodeFromParent(HierarchyConstraints node)",
    "_BuildNodeTable(Widget w, HierarchyConstraints node, Cardinal * current_index)",
):
    start = source.index(signature)
    end = source.find("\n/*", start)
    if end == -1:
        end = len(source)
    region = source[start:end]
    if "int i" in region or "int num" in region:
        raise SystemExit(f"signed child-count index remains in {signature}")
assert "static void BumpChildren(HierarchyConstraints *, Cardinal, Cardinal);" in source
assert "for (i = *num; i > 0; )" in source
assert "for (j = *num; j > i; )" in source
print("Hierarchy child-count paths use full-width Cardinal indices")
