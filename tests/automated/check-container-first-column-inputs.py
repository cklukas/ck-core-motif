#!/usr/bin/env python3
"""Check dynamic Container first-column sizing validates child state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("GetDynFirstColWidth(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "if (cwid == NULL)" in region
assert "if (c == NULL)" in region

print("PASS dynamic Container first-column sizing validates child inputs")
