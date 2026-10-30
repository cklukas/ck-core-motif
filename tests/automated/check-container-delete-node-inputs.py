#!/usr/bin/env python3
"""Check Container node deletion validates child and constraint inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("DeleteNode(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (cwid == NULL)") < region.index("c = GetContainerConstraint(cwid)")
assert "if (c == NULL)" in region
assert "DeleteNode(target_child->widget_ptr)" in region

print("PASS Container node deletion validates child and constraint inputs")
