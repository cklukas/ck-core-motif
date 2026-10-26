#!/usr/bin/env python3
"""Check accessibility child removal validates the parent array invariant."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmAccessibilityRemove(")
end = source.index("\n}", start) + 2
region = source[start:end]

guard = "if (parent->children == NULL || index >= parent->child_count ||\n        parent->child_count > parent->child_capacity ||\n        parent->children[index] != root)"
assert guard in region
assert region.index(guard) < region.index("--parent->child_count;")
assert "if (index < parent->child_count)" in region
assert "parent->child_count - index) * sizeof(*parent->children)" in region
print("PASS accessibility child removal validates array bounds")
