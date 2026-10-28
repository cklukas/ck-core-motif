#!/usr/bin/env python3
"""Ensure converted Mrm callbacks are written through the translated descriptor."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("case MrmRtypeCallback:")
end = text.index("case MrmRtypeSingleFloat:", start)
body = text[start:end]
assert "cbptr = Urm__CW_TranslateOldCallback" in body
assert "callbacks = (XtCallbackRec *)cbptr->item;" in body
assert "((RGMCallbackDescPtr)val)->item" not in body
print("Mrm converted callbacks use the translated destination")
