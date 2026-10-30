#!/usr/bin/env python3
"""Check Container marquee recalculation validates container and anchor state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("RecalcMarquee(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "&&\n\t\t\t    cw->container.anchor_cwid" in region
print("PASS Container marquee recalculation validates widget and anchor state")
