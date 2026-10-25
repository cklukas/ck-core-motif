#!/usr/bin/env python3
"""Check command-palette item input and allocation guards."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/CommandPalette.c"
text = source.read_text(encoding="utf-8")

start = text.index("void XmCommandPaletteSetItems")
end = text.index("Cardinal XmCommandPaletteGetItemCount", start)
body = text[start:end]
assert "if (!items)" in body
assert "if (!palette->palette.items)" in body
assert "XtWarning" in body
assert "palette->palette.items[i].id" in body
copy = text[text.index("static XmString copy_string"):text.index("static Boolean contains_folded", text.index("static XmString copy_string"))]
assert "if (!text) return NULL;" in copy
rebuild = text[text.index("static void rebuild"):start]
assert "palette->palette.item_count && !palette->palette.visible" in rebuild
assert "unable to allocate visible item index" in rebuild
assert "if (!label) continue;" in rebuild
assert "if (!shortcut)" in rebuild
assert "if (!combined)" in rebuild
assert "if (display)" in rebuild
assert "SIZE_MAX - 3" in rebuild
print("command-palette item input and allocation failures are guarded")
