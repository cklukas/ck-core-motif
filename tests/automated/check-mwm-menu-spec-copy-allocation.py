#!/usr/bin/env python3
"""Check that MWM menu-spec duplication guards name copying."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("DuplicateMenuSpec (MenuSpec *menuSpec)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (menuSpec == NULL)" in region
assert "XtNewString(menuSpec->name)" in region
assert "if (menuSpec->name != NULL && newMenuSpec->name == NULL)" in region
assert "XtFree((char *)newMenuSpec);" in region
print("PASS MWM menu-spec copying guards name allocation")
