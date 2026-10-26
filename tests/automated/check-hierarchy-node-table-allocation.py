#!/usr/bin/env python3
"""Check hierarchy node-table growth fits the Xt allocator size."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Hierarchy.c").read_text(
    encoding="utf-8"
)

assert "SIZE_MAX / node_count" in source
assert "UINT_MAX / node_count" in source
assert "new_table == NULL" in source
print("Hierarchy node-table allocation is bounded")
