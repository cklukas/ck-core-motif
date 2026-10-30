#!/usr/bin/env python3
"""Check Container cell-size dispatch validates its widget input."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("SetCellSizes(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("CtrSpatialStyleIsGRID(wid)")
assert "return;" in region

print("PASS Container cell-size dispatch validates widget input")
