#!/usr/bin/env python3
"""Check that List item-selection APIs validate public inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
for function, item_guard in (("XmListSelectItem(", True),
                             ("XmListDeselectItem(", True),
                             ("XmListDeselectPos(", False)):
    start = source.rindex(function)
    body = source[start:source.index("\n}\n", start)]
    assert "if (w == NULL" in body
    if item_guard:
        assert "item == NULL" in body
    assert body.index("_XmWidgetToAppContext(w)") > body.index("if (w == NULL")
print("PASS list item-selection APIs validate inputs")
