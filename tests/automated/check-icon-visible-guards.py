#!/usr/bin/env python3
"""Check icon visibility validates client and icon-box state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "Boolean IconVisible (ClientData *pCD)" in text
assert "if (pCD == NULL || pCD->pSD == NULL)" in text
assert "P_ICON_BOX(pCD)->clipWidget == NULL" in text
assert "P_ICON_BOX(pCD)->scrolledWidget == NULL" in text
print("icon visibility validates client and icon-box state")
