#!/usr/bin/env python3
"""Check minimized-client icon placement rejects invalid conversion results."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinState.c"
text = source.read_text(encoding="utf-8")

assert "ICON_PLACE(pCD) < 0" in text
assert "ICON_PLACE(pCD) >= pWS->IPData.totalPlaces" in text
assert "ICON_PLACE(pCD) = NO_ICON_PLACE;" in text
print("minimized-client icon placement rejects invalid converted indices")
