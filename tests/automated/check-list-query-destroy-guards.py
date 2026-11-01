#!/usr/bin/env python3
"""Check public XmList queries fail closed during widget teardown."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)


def body(name: str, next_name: str) -> str:
    start = source.index(name)
    return source[start : source.index(next_name, start)]


for current, following in (
    ("XmListItemExists(", "XmListItemPos("),
    ("XmListItemPos(", "XmListGetKbdItemPos("),
    ("XmListGetKbdItemPos(", "XmListSetKbdItemPos("),
    ("XmListGetMatchPos(", "ListScrollUp("),
    ("XmListGetSelectedPos(", "XmListSetHorizPos("),
    ("XmListYToPos(", "XmListPosToBounds("),
    ("XmListPosToBounds(", "XmListUpdateSelectedList("),
    ("XmListPosSelected(", "XmCreateList("),
):
    region = body(current, following)
    assert "lw->core.being_destroyed" in region
    assert region.index("lw->core.being_destroyed") < region.index(
        "_XmWidgetToAppContext"
    )

update_region = body("XmListUpdateSelectedList(", "XmListPosSelected(")
assert "lw->core.being_destroyed" in update_region
assert update_region.index("lw->core.being_destroyed") < update_region.index(
    "_XmWidgetToAppContext"
)

print("PASS XmList query and selection-update APIs guard teardown before list access")
