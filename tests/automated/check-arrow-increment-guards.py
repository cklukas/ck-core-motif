#!/usr/bin/env python3
"""Check icon-box increment callbacks validate their state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pIBD == NULL || pIBD->pCD_iconBox == NULL" in text
assert "pCD == NULL || pCD->thisIconBox == NULL" in text
assert "pCD->thisIconBox->vScrollBar == NULL" in text
assert "pCD->thisIconBox->hScrollBar == NULL" in text
assert "pCD->heightInc <= 0 ||" in text
print("icon-box increment callbacks validate client and scrollbar state")
