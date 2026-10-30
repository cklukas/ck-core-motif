#!/usr/bin/env python3
"""Check Container icon-header updates validate header data and restore state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("UpdateIconHeader(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "if (icon_header == NULL || cw->container.detail_heading == NULL" in region
assert "cw->container.self = False" in region
assert "assert (cw->container.detail_heading_count > 0)" not in region

print("PASS Container icon-header updates validate header data and restore state")
