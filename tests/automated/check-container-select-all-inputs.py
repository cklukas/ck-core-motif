#!/usr/bin/env python3
"""Check Container select-all validates its widget input."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("SelectAllCwids(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "return(False);" in region

print("PASS Container select-all validates widget input")
