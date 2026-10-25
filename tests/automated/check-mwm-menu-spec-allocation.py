#!/usr/bin/env python3
"""Check that MWM menu-spec naming is allocation-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(encoding="utf-8")
start = source.index("MakeMenuSpec (String menuName")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (menuName == NULL)" in region
assert "menuSpec->name = XtNewString(menuName);" in region
assert "if (menuSpec->name == NULL)" in region
assert "XtFree((char *)menuSpec);" in region
print("PASS MWM menu-spec name allocation is guarded")
