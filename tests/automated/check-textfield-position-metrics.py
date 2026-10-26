#!/usr/bin/env python3
"""Ensure TextField position metrics are checked before Position narrowing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("GetXYFromPos(XmTextFieldWidget tf,")
end = source.index("\nstatic Boolean \nCurrentCursorState", start)
region = source[start:end]

assert "int64_t baseline_y" in region
assert "(int64_t)tf->text.margin_top" in region
assert "baseline_y < SHRT_MIN || baseline_y > SHRT_MAX" in region
assert region.count("*y = (Position)baseline_y;") == 2
assert "*y += tf->primitive" not in region

print("TextField position metrics are checked before Position conversion")
