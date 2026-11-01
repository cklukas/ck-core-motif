#!/usr/bin/env python3
"""Check I18List convenience APIs fail closed and release search strings."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/I18List.c").read_text(
    encoding="utf-8"
)

def body(name: str, next_name: str) -> str:
    start = source.index(name, source.index("Exported Functions."))
    end = source.index(next_name, start) if next_name else len(source)
    return source[start:end]


for current, following in (
    ("XmI18ListGetSelectedRows(", "XmI18ListDoSearch("),
    ("XmI18ListDoSearch(", "XmI18ListFindRow("),
    ("XmI18ListFindRow(", "static int\nFirstSelectedRow("),
    ("Xm18IListUnselectItem(", "XmI18ListToggleRow("),
    ("XmI18ListToggleRow(", "CopyColumnTitles("),
    ("XmI18ListMakeRowVisible(", ""),
):
    region = body(current, following)
    assert "core.being_destroyed" in region, current

for current, following in (
    ("XmI18ListDoSearch(", "XmI18ListFindRow("),
    ("XmI18ListFindRow(", "static int\nFirstSelectedRow("),
):
    region = body(current, following)
    assert "foundit = Search" in region, current
    assert region.index("XmStringFree(xms)") > region.index("foundit = Search"), current
    assert "if (xms == NULL)" in region, current

print("PASS I18List convenience APIs guard teardown and free search strings")
