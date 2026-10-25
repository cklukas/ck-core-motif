#!/usr/bin/env python3
"""Check that MWM menu-item construction is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("MakeMenuItem (String label")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "XtCalloc (1, sizeof (MenuItem))" in region
assert "if (label != NULL)" in region
assert "if (menuItem->label == NULL)" in region
assert "if (menuItem->accelText == NULL)" in region
assert "if (menuItem->wmFuncArgs == NULL)" in region
assert "FreeMenuItem(menuItem);" in region
print("PASS MWM menu-item construction is transactional")
