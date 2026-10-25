#!/usr/bin/env python3
"""Check default font-list cache growth is transactional."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("DefaultSystemFontList(")
end = text.index("void\n_XmFreeDefaultRenderTable", start)
block = text[start:end]

for needle in (
    "SystemFontList *new_font_lists",
    "if (new_font_lists == NULL)",
    "maxnsFontLists = new_maxns_font_lists;",
):
    if needle not in block:
        raise SystemExit(f"font cache growth is missing {needle}")

print("default font-list cache handles allocation failure")
