#!/usr/bin/env python3
"""Check that MWM menu-button counting cannot wrap before allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("MenuSpec *MakeMenu")
end = source.index("\n}\n", start) + 2
region = source[start:end]

assert region.count("if (n == UINT_MAX)") == 2
assert region.count("Menu button array is too large for menu %s") == 3
assert "n > UINT_MAX / sizeof(*menuSpec->menuButtons)" in region
assert "menuSpec->menuButtonSize = n;" in region
print("PASS MWM menu-button count and Cardinal allocation widths are bounded")
