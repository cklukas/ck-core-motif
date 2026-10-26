#!/usr/bin/env python3
"""Check that quoted MWM menu names are copied only after allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = source.index("static void ParseMenuItemName (unsigned char **linePP, MenuItem *menuItem)\n{")
end = source.index("\n#endif /* !defined(WSM)", start)
region = source[start:end]
assert "nameLength = (size_t)(endquote - lineP);" in region
assert "if (nameLength == SIZE_MAX || nameLength >= UINT_MAX)" in region
assert "menuItem->clientCommandName = XtMalloc((Cardinal)nameLength + 1);" in region
assert "if (menuItem->clientCommandName == NULL)" in region
assert "memcpy(menuItem->clientCommandName, lineP, nameLength);" in region
print("PASS MWM quoted menu-name allocation is guarded")
