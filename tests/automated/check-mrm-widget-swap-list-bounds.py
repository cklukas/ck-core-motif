#!/usr/bin/env python3
"""Check byte-swapped widget lists are bounded by their record size."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIswap.c").read_text(
    encoding="utf-8"
)
for required in (
    "UrmSwapWidgetSpanValid",
    "UrmSwapWidgetListValid",
    "widget_rec->size < RGMWidgetRecordHdrSize",
    "sizeof(RGMArgListDesc) -\n\t\t\t\t  sizeof(RGMArgument)",
    "sizeof(RGMChildrenDesc) -\n\t\t\t\t  sizeof(RGMChildDesc)",
):
    assert required in source, required

for offset, count in (("arglist_offs", "arg_list->count"),
                      ("children_offs", "child_list->count")):
    start = source.index(f"if (widget_rec->{offset} > 0)")
    body = source[start:source.index("\n  /*", start)]
    prefix = body.index("UrmSwapWidgetSpanValid")
    loop = body.index("for ( ndx=0")
    assert prefix < loop
    assert f"UrmSwapWidgetListValid(widget_rec, widget_rec->{offset}" in body
    assert count in body

print("Mrm byte-swapped widget list conversion validates record spans")
