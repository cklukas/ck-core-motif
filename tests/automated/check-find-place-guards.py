#!/usr/bin/env python3
"""Check FindIconPlace and root conversion handle missing placement state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

assert "pCD == NULL || pIPD == NULL" in text
assert "pIPD->placeList == NULL" in text
assert "pIPD->placementColX == NULL || pIPD->placementRowY == NULL" in text
print("icon-place search and root conversion validate placement state")
