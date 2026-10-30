#!/usr/bin/env python3
"""Check Container selection dispatch validates widget, event, and constraint."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("StartSelect(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL || event == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "c != NULL" in region

print("PASS Container selection dispatch validates widget, event, and constraint")
