#!/usr/bin/env python3
"""Check icon-box size checks validate widgets and increments."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "Boolean CheckIconBoxSize (IconBoxData *pIBD)" in text
assert "pIBD == NULL || pIBD->bBoardWidget == NULL" in text
assert "pIBD->pCD_iconBox->widthInc <= 0" in text
assert "pIBD->pCD_iconBox->heightInc <= 0" in text
print("icon-box size checks validate widgets and increments")
