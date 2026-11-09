#!/usr/bin/env python3
"""Verify MWM menu automation replies never serialize unset metadata."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)

icon_start = source.index("static void\nGetIconMenuItemSelectInfo")
window_start = source.index("static void\nGetWindowItemSelectInfo")
check_start = source.index("static void\nGetItemCheckInfo")
icon = source[icon_start:window_start]
window = source[window_start:check_start]
check = source[check_start:source.index(
    "/*----------------------------------------------------------------------*", check_start + 20
)]

for region in (icon, window):
    assert "itemName[n][0] = '\\0';" in region
    assert "sensitive[n] = FALSE;" in region
    assert "itemY[n] = 0;" in region
    assert "reply = PackCARD32 (reply, (CARD32)itemY[n]);" in region

assert "unsigned int menuItemCount;" in icon
assert "unsigned int menuItemCount;" in check
for flag in ("titleFound", "systemFound", "minimizeFound", "maximizeFound"):
    assert f"Boolean {flag} = False;" in check

print("PASS MWM automation menu replies initialize every serialized field")
