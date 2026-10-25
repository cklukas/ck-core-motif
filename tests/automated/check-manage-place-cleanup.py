#!/usr/bin/env python3
"""Check client withdrawal bounds-checks icon placement cleanup."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmManage.c"
text = source.read_text(encoding="utf-8")

assert "pWsTmp->IPData.placeList != NULL" in text
assert "pWsc->iconPlace >= 0" in text
assert "pCD->pSD->pActiveWS->IPData.placeList != NULL" in text
assert "ICON_PLACE(pCD) >= 0" in text
print("client withdrawal validates icon placement cleanup")
