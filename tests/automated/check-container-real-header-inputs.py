#!/usr/bin/env python3
"""Check Container real-header lookup validates widget and child state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("GetRealIconHeader(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "XtIsComposite" in region
assert "composite.num_children == 0" in region

print("PASS Container real-header lookup validates widget and child state")
