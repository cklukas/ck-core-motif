#!/usr/bin/env python3
"""Check Container node traversal validates container and widget state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
first_start = source.rindex("GetFirstNode(\n")
first_region = source[first_start : source.index("\n}\n", first_start)]
assert "if (cw == NULL || cw->container.first_node == NULL)" in first_region

next_start = source.rindex("GetNextNode(\n")
next_region = source[next_start : source.index("\n}\n", next_start)]
assert "if (start_node->widget_ptr == NULL)" in next_region
assert "if (cw == NULL)" in next_region

print("PASS Container node traversal validates container and widget state")
