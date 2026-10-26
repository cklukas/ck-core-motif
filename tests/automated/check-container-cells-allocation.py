#!/usr/bin/env python3
"""Ensure Container CELLS geometry allocation fits Xt's size type."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("if (CtrSpatialStyleIsCELLS(cw))", source.index("Must allocate array"))
end = source.index("\n    else\n", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(XmContainerCwidCellInfoRec)" in region
assert "UINT_MAX / sizeof(XmContainerCwidCellInfoRec)" in region
assert region.count("return;") >= 1

print("Container CELLS geometry allocation checks host and Xt limits")
