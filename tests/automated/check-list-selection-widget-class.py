#!/usr/bin/env python3
"""Check public List selection APIs validate their widget class."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(encoding="utf-8")
for name in (
    "XmListSelectItem(", "XmListSelectPos(", "XmListDeselectItem(",
    "XmListDeselectPos(", "XmListDeselectAllItems("
):
    start = source.rindex(name)
    region = source[start:source.index("\n}\n", start)]
    assert "!XmIsList(w)" in region, name
    assert region.index("!XmIsList(w)") < region.index("_XmWidgetToAppContext(w)"), name
print("PASS List selection APIs validate widget class before list access")
