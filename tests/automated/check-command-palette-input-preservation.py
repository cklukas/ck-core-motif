#!/usr/bin/env python3
"""Check invalid CommandPalette replacements preserve existing items."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
start = source.index("void XmCommandPaletteSetItems")
end = source.index("Cardinal XmCommandPaletteGetItemCount", start)
function = source[start:end]
free = function.index("free_items(palette)")
null_guard = function.index('"XmCommandPaletteSetItems: NULL item array"')
size_guard = function.index("SIZE_MAX / sizeof(*palette->palette.items)")
assert null_guard < free
assert size_guard < free
assert "if (!count) { rebuild(palette); return; }" in function
print("CommandPalette rejects invalid replacements before replacing existing items")
