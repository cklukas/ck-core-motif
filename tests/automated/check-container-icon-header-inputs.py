#!/usr/bin/env python3
"""Check Container icon-header creation validates widget and parent state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("CreateIconHeader(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "if (XtParent(wid) != NULL)" in region

print("PASS Container icon-header creation validates widget and parent state")
