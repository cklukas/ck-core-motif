#!/usr/bin/env python3
"""Check Container outline layout validates child and parent state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("LayoutOutlineDetail(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert region.count("if (node->widget_ptr == NULL)") == 1
assert region.count("if (cwid == NULL)") == 1
assert region.count("if (c == NULL)") == 2
assert "pc = GetContainerConstraint(c->entry_parent)" in region

print("PASS Container outline layout validates child and parent state")
