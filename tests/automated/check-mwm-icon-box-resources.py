#!/usr/bin/env python3
"""Check MWM icon-box GC and stipple resource handling."""

from pathlib import Path


source = Path("clients/mwm/WmIconBox.c").read_text(encoding="utf-8")

for marker, resource in (
    ("void MakeShrinkWrapIconsGC", "shrinkWrapGC == NULL"),
    ("void MakeFadeIconGC", "fadeIconGC == NULL"),
):
    start = source.index(marker)
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "pSD == NULL" in body
    assert resource in body
    assert "return;" in body

fade_start = source.index("void MakeFadeIconGC")
fade_end = source.index("END OF FUNCTION", fade_start)
fade_body = source[fade_start:fade_end]
assert "tmpFontClipMask == None || greyedPixmap == None" in fade_body
assert "XFreeGC (DISPLAY, pSD->fadeIconGC)" in fade_body
assert "XFreeGC (DISPLAY, pSD->fadeIconTextGC)" in fade_body

print("PASS MWM icon-box resources are failure-safe")
