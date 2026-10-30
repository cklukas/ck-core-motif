#!/usr/bin/env python3
"""Check Container expand/collapse validates outline state inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
for signature in ("ExpandCwid(\n", "CollapseCwid(\n"):
    start = source.rindex(signature)
    region = source[start:source.index("\n}", start)]
    assert "if (cwid == NULL)" in region
    assert "if (cw == NULL)" in region
    assert "if (c == NULL)" in region
    assert "if (child_node->widget_ptr == NULL)" in region
print("PASS Container expand/collapse validates outline state inputs")
