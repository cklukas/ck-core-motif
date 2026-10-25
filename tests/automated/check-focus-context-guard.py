#!/usr/bin/env python3
"""Check focus callbacks reject null client contexts."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "wmGD.windowContextType, (caddr_t *)&pCD)) && pCD != NULL" in text
print("focus callback validates the client context before icon-box access")
