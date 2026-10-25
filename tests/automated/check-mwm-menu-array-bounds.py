#!/usr/bin/env python3
"""Check MWM menu query loops respect fixed array capacities."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)
icon = source[source.index("static void\nGetIconMenuItemSelectInfo") : source.index("static void\nGetWindowItemSelectInfo")]
window = source[source.index("static void\nGetWindowItemSelectInfo") : source.index("/*----------------------------------------------------------------------*\n |                             GetItemCheckInfo", source.index("static void\nGetWindowItemSelectInfo"))]
check = source[source.index("static void\nGetItemCheckInfo") : source.index("/*----------------------------------------------------------------------*", source.index("static void\nGetItemCheckInfo") + 20)]
assert "n < menuSpec->menuButtonCount && n < MAX_MENU_ITEMS" in icon
assert "n < menuSpec->menuButtonCount && n < MAX_MENU_ITEMS" in window
assert "i < menuSpec->menuButtonCount && i < MAX_MENU_ITEMS" in check

print("MWM menu query loops respect fixed array bounds")
