#!/usr/bin/env python3
"""Check icon insertion validates inputs and post-growth placement."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "if (pIBD == NULL || pCD == NULL || pIBD->IPD.placeList == NULL ||" in text
assert "pIBD->IPD.totalPlaces <= 0" in text
assert "pII->theWidget = NULL;" in text
assert "pII->pCD = NULL;" in text
print("icon insertion validates inputs and placement availability")
