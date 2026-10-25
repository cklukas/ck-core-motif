#!/usr/bin/env python3
"""Check icon-list growth rejects a missing source list before copying."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pIBD->IPD.totalPlaces > 0 && pIBD->IPD.placeList == NULL" in text
assert "memcpy (pTmp, pIBD->IPD.placeList" in text
print("icon-list growth validates source storage before copying")
