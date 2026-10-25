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
assert window.count("if (menuItemCount > MAX_MENU_ITEMS)") == 1
assert check.count("if (menuItemCount > MAX_MENU_ITEMS)") == 1
assert icon.count("if (menuItemCount > MAX_MENU_ITEMS)") == 1
query = source[source.index("case WINDOW_MENU_ITEM_SELECT_INFO") : source.index("case WINDOW_DEICONIFY_INFO")]
check_query = source[source.index("case WINDOW_MENU_ITEM_CHECK_INFO") : source.index("case ICON_MOVE_INFO")]
icon_query = source[source.index("case ICON_MENU_ITEM_SELECT_INFO") :]
for region in (query, check_query, icon_query):
    assert "menuButtons[n].menuItem" in region
    assert "menu_item && menu_item->label" in region

print("MWM menu query loops respect fixed array bounds")
