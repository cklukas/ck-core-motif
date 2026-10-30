#!/usr/bin/env python3
"""Check Container node resequencing validates container and node state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static void ContainerResequenceNodes")
region = source[start : source.index("\n}\n", start)]
assert "if (cw == NULL)" in region
assert "if (c == NULL || c->node_ptr == NULL)" in region
assert "if (node->widget_ptr == NULL)" in region
assert "if (c == NULL)" in region

print("PASS Container node resequencing validates container and node state")
