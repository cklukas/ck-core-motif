#!/usr/bin/env python3
"""Check icon-info deletion validates client and placement storage."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void DeleteIconInfo (IconBoxData *pIBD, ClientData *pCD)" in text
assert "pIBD == NULL || pCD == NULL" in text
assert "pIBD->IPD.placeList == NULL" in text
assert "pIBD->IPD.totalPlaces <= 0" in text
print("icon-info deletion validates client and placement storage")
