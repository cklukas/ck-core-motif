#!/usr/bin/env python3
"""Check Container outline-button sizing validates widget and allocation."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("SizeOutlineButton(\n")
region = source[start : source.index("\n}\n", start)]
assert region.index("if (wid == NULL)") < region.index("cw = (XmContainerWidget)wid")
assert "if (cw->container.size_ob == NULL)" in region
assert "cw->container.self = False" in region

print("PASS Container outline-button sizing validates widget and allocation")

column_start = source.rindex("GetOutlineColumnWidth(")
column_region = source[column_start : source.index("\n}\n", column_start)]
assert "cw = (XmContainerWidget)wid" in column_region
assert column_region.index("if (wid == NULL") < column_region.index("cw = (XmContainerWidget)wid")
