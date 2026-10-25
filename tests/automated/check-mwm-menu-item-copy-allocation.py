#!/usr/bin/env python3
"""Check that MWM menu-item duplication is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("DuplicateMenuItems (MenuItem *menuItems)\n{")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "XtCalloc(1, sizeof(MenuItem))" in region
assert "if (newMenuItem->label == (String)NULL)" in region
assert "if (newMenuItem->accelText == (String)NULL)" in region
assert "if (newMenuItem->wmFuncArgs == (String)NULL)" in region
assert "FreeMenuItem(newMenuItem);" in region
assert "FreeMenuItem(returnMenuItem);" in region
print("PASS MWM menu-item copying is transactional")
