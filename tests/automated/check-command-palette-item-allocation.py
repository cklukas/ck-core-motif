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
print("command-palette item input and allocation failures are guarded")
