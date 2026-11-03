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
    "sizeof(RGMCallbackDesc) -\n\t\t\t      sizeof(RGMCallbackItem)",
    "(size_t)callb_desc->count + 1",
    "offsetof(RGMResourceDesc, key) + 1",
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

callback_start = source.index("Urm__SwapRGMCallbackDesc")
callback_body = source[callback_start:source.index("\n\f", callback_start)]
assert "UrmSwapWidgetSpanValid(widget_rec, callback_offs" in callback_body
assert "callb_desc->count > RGMListSizeMax" in callback_body
assert "return MrmFAILURE;" in callback_body

widget_start = source.index("Urm__SwapRGMWidgetRecord")
widget_body = source[widget_start:]
assert "case MrmRtypeCallback:" in widget_body
assert "case MrmRtypeResource:" in widget_body
assert "offset = ((char *)widget_rec" not in widget_body

print("Mrm byte-swapped widget list conversion validates record spans")
