#!/usr/bin/env python3
"""Check virtual-space expansion validates dimensions and arithmetic."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pIBD == NULL || pIBD->pCD_iconBox == NULL" in text
assert "newWidth < 0 || newHeight < 0" in text
assert "widthInc <= 0" in text
assert "rows > INT_MAX / columns" in text
print("virtual-space expansion validates dimensions and arithmetic")
