#!/usr/bin/env python3
"""Check MoveIconInfo rejects invalid placement state before indexing."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

assert "pIPD == NULL || pIPD->placeList == NULL" in text
assert "p1 < 0 || p2 < 0" in text
assert "p1 >= pIPD->totalPlaces || p2 >= pIPD->totalPlaces" in text
assert "pIPD->placeList[p1].pCD == NULL" in text
assert "if (pWsc != NULL)" in text
print("icon-info moves validate placement storage, indices, and source clients")
