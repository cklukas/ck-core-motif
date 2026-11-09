#!/usr/bin/env python3
"""Check command-palette item input and allocation guards."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/CommandPalette.c"
text = source.read_text(encoding="utf-8")

start = text.index("void XmCommandPaletteSetItems")
end = text.index("Cardinal XmCommandPaletteGetItemCount", start)
body = text[start:end]
assert "if (!items)" in body
assert "replacement = (XmCommandPaletteStoredItem *)XtCalloc(" in body
assert "if (!replacement)" in body
assert "XtWarning" in body
assert "replacement[i].id" in body
copy = text[text.index("static XmString copy_string"):text.index("static Boolean contains_folded", text.index("static XmString copy_string"))]
assert "return value ? XmStringCopy(value) : XmStringCreateLocalized(\"\");" in copy
rebuild = text[text.index("static void rebuild"):start]
assert "palette->palette.item_count && !palette->palette.visible" in rebuild
assert "unable to allocate visible item index" in rebuild
assert "if (!label)" in rebuild
assert "continue;" in rebuild
assert "if (!shortcut)" in rebuild
assert "make_display_string(" in rebuild
assert "if (display)" in rebuild
assert "if (!replacement[i].id || !replacement[i].label ||" in body
assert "unable to copy item text" in body
assert "free_items(palette);" in body
assert "palette->palette.items = replacement;" in body
assert "palette->palette.item_count = count;" in body
print("command-palette item input and allocation failures are guarded")
