#!/usr/bin/env python3
"""Check Container hit testing validates its widget input."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("ObjectAtPoint(Widget wid")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "return(NULL);" in region

print("PASS Container hit testing validates widget input")
