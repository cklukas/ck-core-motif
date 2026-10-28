#!/usr/bin/env python3
"""Ensure Mrm reader vector/count branches reject signed negatives."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_ConvertValue (Widget")
body = text[start:]
assert "vecptr->count < 0 || vecptr->count > RGMListSizeMax" in body
assert "fontlist->count < 0 || fontlist->count > RGMListSizeMax" in body
assert "vecptr->count < 0 || vecptr->count > RGMListSizeMax" in body
print("Mrm reader rejects negative vector and font-list counts")
