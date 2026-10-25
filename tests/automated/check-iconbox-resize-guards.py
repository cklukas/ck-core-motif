#!/usr/bin/env python3
"""Check icon-box resize validates widgets, policy, dimensions, and sizes."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pCD->thisIconBox->clipWidget == NULL" in text
assert "pSD->iconBoxSBDisplayPolicy == NULL" in text
assert "pIPD->placementCols <= 0 || pIPD->placementRows <= 0" in text
assert "newWidth < 0 || newHeight < 0" in text
print("icon-box resize validates widgets, policy, dimensions, and sizes")
