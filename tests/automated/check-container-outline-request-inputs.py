#!/usr/bin/env python3
"""Check Container outline geometry requests validate their inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("RequestOutlineDetail(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL || geo_desired == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "return;" in region

print("PASS Container outline geometry requests validate inputs")
