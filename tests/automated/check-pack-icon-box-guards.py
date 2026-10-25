#!/usr/bin/env python3
"""Check icon-box packing validates widgets, increments, and storage."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pIBD->bBoardWidget == NULL" in text
assert "pIBD->clipWidget == NULL" in text
assert "pIBD->pCD_iconBox->widthInc <= 0" in text
assert "pIBD->pCD_iconBox->heightInc <= 0" in text
assert "pIBD->IPD.placeList == NULL" in text
print("icon-box packing validates widgets, increments, and storage")
