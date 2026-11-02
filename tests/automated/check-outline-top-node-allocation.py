#!/usr/bin/env python3
"""Check Outline initialization publishes its resized top node safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Outline.c").read_text(
    encoding="utf-8"
)

assert "HierarchyConstraints top_node" in source
assert "if (top_node == NULL)" in source
assert 'XtError("Unable to allocate Outline top-node constraint");' in source
assert "XmHierarchy_top_node(ow) = top_node" in source
print("PASS XmOutline top-node allocation failure is explicit and terminal")
