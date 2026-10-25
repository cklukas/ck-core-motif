#!/usr/bin/env python3
"""Check that List replacement APIs validate public inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
for helper, marker in (("APIReplaceItems", "old_items[i] == NULL"),
                       ("APIReplaceItemsPos", "new_items[i] == NULL")):
    start = source.index("static void\n" + helper)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "item_count <= 0" in region
    assert marker in region
for function in ("XmListReplaceItems(", "XmListReplaceItemsUnselected(",
                 "XmListReplaceItemsPos(", "XmListReplaceItemsPosUnselected("):
    start = source.rindex(function)
    body = source[start:source.index("\n}\n", start)]
    assert "if (w == NULL)" in body
print("PASS list replacement APIs validate widget and item inputs")
