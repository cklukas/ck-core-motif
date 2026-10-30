#!/usr/bin/env python3
"""Check small-cell sizing validates widget and first-node state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("SetSmallCellSizes(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "while (node && node->widget_ptr == NULL)" in region
assert "if (node == NULL)" in region

print("PASS small-cell sizing validates widget and first-node state")
