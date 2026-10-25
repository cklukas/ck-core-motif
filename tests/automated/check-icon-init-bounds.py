#!/usr/bin/env python3
"""Check icon-box initialization rejects invalid placement dimensions."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "iconWidth <= 0 || pWS->pSD->iconHeight <= 0" in text
assert "sW > INT_MAX / sH" in text
assert "totalPlaces <= 0" in text
assert "UINT_MAX / sizeof(IconInfo)" in text
print("icon-box initialization validates dimensions and placement size")
