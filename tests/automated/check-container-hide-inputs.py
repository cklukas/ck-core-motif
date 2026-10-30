#!/usr/bin/env python3
"""Check Container outline hiding validates constraints and child nodes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("HideCwid(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (c == NULL)") < region.index("c->related_cwid")
assert "if (child_node->widget_ptr == NULL)" in region
print("PASS Container outline hiding validates constraints and child nodes")
