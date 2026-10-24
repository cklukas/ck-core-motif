#!/usr/bin/env python3
"""Check menu-label parsing handles empty and multibyte labels safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("static void ParseMenuItemName (unsigned char **linePP, MenuItem *menuItem)\n{")
end = source.index("#endif /* !defined(WSM)", start)
region = source[start:end]
assert "int chlen = 0;" in region
assert "memcpy(menuItem->clientCommandName, lineP, nameLength);" in region
assert "menuItem->clientCommandName[nameLength] = '\\0';" in region
assert "strlen(menuItem->clientCommandName)+1" not in region
print("mwm menu labels are explicitly terminated")
