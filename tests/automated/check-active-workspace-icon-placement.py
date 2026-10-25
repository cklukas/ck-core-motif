#!/usr/bin/env python3
"""Check that non-WSM icon placement tolerates a missing active workspace."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c"
text = source.read_text()
pos = text.index("if (pCD->pSD->pActiveWS != NULL)")
block = text[pos:text.index("#endif /* WSM */", pos)]
assert "FindIconPlace" in block
assert "pCD->iconPlace = NO_ICON_PLACE" in block
assert "pCD->iconX = tmpIconX" in block
assert "pCD->iconY = tmpIconY" in block
print("icon hint placement validates the active workspace")
