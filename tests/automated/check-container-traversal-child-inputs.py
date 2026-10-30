#!/usr/bin/env python3
"""Check Container next/previous traversal validates child constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
for signature in ("GetNextTraversalWidget(\n", "GetPrevTraversalWidget(\n"):
    start = source.rindex(signature)
    region = source[start:source.index("\n}", start)]
    assert "if (c == NULL || c->node_ptr == NULL)" in region, signature
    assert region.index("if (c == NULL || c->node_ptr == NULL)") < region.index("child_node = c->node_ptr")
print("PASS Container next/previous traversal validates child constraints")
