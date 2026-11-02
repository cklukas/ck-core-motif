#!/usr/bin/env python3
"""Check that title-bar height does not wrap signed font metrics."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("void \nMakeAppearanceResources")
body = source[start:source.index("/* background pixmap */", start)]

assert "long titleHeight;" in body
assert "(long)(pAData->font)->ascent" in body
assert "(long)(pAData->font)->descent" in body
assert "titleHeight <= 0 || titleHeight > (long)UINT_MAX" in body
assert 'Warning("Invalid title font metrics")' in body
assert "pAData->titleHeight = 16;" in body
assert "pAData->titleHeight = (unsigned int)titleHeight;" in body

print("MWM title height validates signed font metrics before conversion")
