#!/usr/bin/env python3
"""Check Container hit-testing skips malformed child nodes safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ObjectAtPoint(Widget wid")
region = source[start:source.index("\n}", start)]
assert "if (node->widget_ptr == NULL)" in region
assert "if (c == NULL)" in region
assert region.index("if (c == NULL)") < region.index("c -> related_cwid")
print("PASS Container hit-testing skips null nodes and constraints")
