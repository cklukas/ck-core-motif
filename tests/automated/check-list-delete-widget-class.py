#!/usr/bin/env python3
"""Check public List deletion APIs validate their widget class."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(encoding="utf-8")
for name in (
    "XmListDeleteItem(", "XmListDeleteItems(", "XmListDeletePositions(",
    "XmListDeletePos(", "XmListDeleteItemsPos(", "XmListDeleteAllItems("
):
    start = source.index(name)
    region = source[start:source.index("\n}\n", start)]
    assert "!XmIsList(w)" in region, name
    assert region.index("!XmIsList(w)") < region.index("_XmWidgetToAppContext(w)"), name
print("PASS List deletion APIs validate widget class before list access")
