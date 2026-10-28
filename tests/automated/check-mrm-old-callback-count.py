#!/usr/bin/env python3
"""Ensure legacy Mrm callback translation allocates its terminator item."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_TranslateOldCallback (OldRGMCallbackDescPtr")
body = text[start:text.index("\n}\n", start)]
assert "oldptr->count == INT_MAX" in body
assert "oldptr->validation != URMCallbackDescriptorValid" in body
record_start = text.index("UrmCallbackRecordValid(RGMWidgetRecordPtr")
record_body = text[record_start:text.index("\n}\n", record_start)]
assert "widgetrec == NULL" in record_body
assert record_body.count("desc->validation != URMCallbackDescriptorValid") == 2
assert "item_count = (size_t)oldptr->count + 1" in body
assert "item_count * sizeof(RGMCallbackItem)" in body
assert "for (ndx = 0; ndx <= cbptr->count; ndx++)" in body
print("Mrm legacy callback terminator allocation is covered")
