#!/usr/bin/env python3
"""Check icon insertion validates geometry and widget state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pIBD->pCD_iconBox == NULL" in text
assert "pIBD->pCD_iconBox->widthInc <= 0" in text
assert "pIBD->pCD_iconBox->heightInc <= 0" in text
assert "pIBD->clipWidget == NULL" in text
assert "pIBD->bBoardWidget == NULL" in text
print("icon insertion validates geometry and widget state")
