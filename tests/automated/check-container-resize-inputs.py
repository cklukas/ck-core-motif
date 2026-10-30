#!/usr/bin/env python3
"""Check Container icon-width resizing validates widget and child state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("ResizeIconWidths(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert region.count("if (cwid == NULL)") == 2
assert region.count("if (c == NULL)") == 2

print("PASS Container icon-width resizing validates widget and child inputs")
