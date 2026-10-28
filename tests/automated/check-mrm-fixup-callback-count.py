#!/usr/bin/env python3
"""Ensure Mrm callback fixup rejects negative descriptor counts."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_FixupCallback (Widget")
body = text[start:text.index("\n}\n", start)]
assert "cbdesc->count < 0" in body
assert "cbdesc->count > RGMListSizeMax" in body
assert "for ( ndx=0 ; ndx<cbdesc->count ; ndx++ )" in body
print("Mrm callback fixup count bounds are validated")
