#!/usr/bin/env python3
"""Check that failed MWM list copies abort custom-menu duplication."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("DuplicateMenuSpec (MenuSpec *menuSpec)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "newMenuSpec->menuItems = DuplicateMenuItems(menuSpec->menuItems);" in region
assert "newMenuSpec->menuItems == (MenuItem *)NULL" in region
assert "newMenuSpec->exclusions = DuplicateMenuExclusions(menuSpec->exclusions);" in region
assert "newMenuSpec->exclusions == (MenuExclusion *)NULL" in region
assert "FreeMenuItem(item);" in region
assert "return (NULL);" in region
print("PASS MWM menu-spec copy failures abort transactionally")
