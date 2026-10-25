#!/usr/bin/env python3
"""Check that MWM push-button array growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("static Boolean SavePBInfo")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "MenuButton *newButtons;" in region
assert "XtRealloc(" in region
assert "if (newButtons == NULL)" in region
assert "topMenuSpec->menuButtons = newButtons;" in region
assert "topMenuSpec->menuButtonSize += MENU_BUTTON_INC" not in region
assert "SIZE_MAX / sizeof(*newButtons)" in region
print("PASS MWM push-button growth is transactional")
