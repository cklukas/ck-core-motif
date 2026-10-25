#!/usr/bin/env python3
"""Check that MWM menu-button array sizing is bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("MenuSpec *MakeMenu (")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "SIZE_MAX / sizeof(*menuSpec->menuButtons)" in region
assert "Menu button array is too large" in region
assert "XtMalloc (n * sizeof(MenuButton))" in region
print("PASS MWM menu-button array sizing is bounded")
