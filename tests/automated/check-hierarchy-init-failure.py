#!/usr/bin/env python3
"""Check Hierarchy construction failure leaves a destroy-safe state."""

from pathlib import Path


source = Path("lib/Xm/Hierarchy.c").read_text()

init_start = source.index("Initialize(Widget req")
init = source[init_start:source.index("ConstraintInitialize(", init_start)]
destroy_start = source.index("Destroy(Widget w)")
destroy = source[destroy_start:source.index("InsertChild(Widget w)", destroy_start)]

assert "XmHierarchy_top_node(hw) = NULL;" in init
assert "XmHierarchy_top_node(hw) = top_node;" in init
assert "if (XmHierarchy_top_node(hw) != NULL)" in destroy

print("Hierarchy initialization failure is destroy-safe")
