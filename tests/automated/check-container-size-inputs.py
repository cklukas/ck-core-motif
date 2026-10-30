#!/usr/bin/env python3
"""Check Container size calculation validates outputs and child state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("GetSize (\n")
region = source[start : source.index("\n}\n", start)]
guard = "if (wid == NULL || pwidth == NULL || pheight == NULL)"
assert region.index(guard) < region.index("cw = (XmContainerWidget)wid")
assert "if (cwid == NULL)" in region
assert "if (c == NULL)" in region

print("PASS Container size calculation validates output and child inputs")
