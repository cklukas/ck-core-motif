#!/usr/bin/env python3
"""Check Container graphics-context updates validate their widget input."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("UpdateGCs(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "XtAllocateGC" in region

print("PASS Container graphics-context updates validate widget input")
