#!/usr/bin/env python3
"""Check Container outline-button refresh validates its widget input."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("ChangeOutlineButtons(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "for (i = 0; i < cw->composite.num_children; i++)" in region

print("PASS Container outline-button refresh validates widget input")
