#!/usr/bin/env python3
"""Ensure command-palette item replacement preserves state on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmCommandPaletteSetItems")
end = source.index("\nCardinal XmCommandPaletteGetItemCount", start)
body = source[start:end]

assert "XmCommandPaletteStoredItem *replacement;" in body
assert "replacement = (XmCommandPaletteStoredItem *)XtCalloc" in body
assert "free_item_array(replacement, i + 1);" in body
assert body.index("if (!replacement[i].id") < body.rindex("free_items(palette);")
assert "palette->palette.items = replacement;" in body
print("PASS CommandPalette item replacement is transactional")
