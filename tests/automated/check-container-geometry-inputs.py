#!/usr/bin/env python3
"""Check Container geometry negotiation validates child request inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("GeometryManager(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (cwid == NULL || desired == NULL)") < region.index("wid = XtParent(cwid)")
assert "if (wid == NULL)" in region
assert "if (c == NULL)" in region
assert "return(XtGeometryNo);" in region

print("PASS Container geometry negotiation validates child request inputs")
