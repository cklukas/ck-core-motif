#!/usr/bin/env python3
"""Check bounded growth of the per-display default font-list cache."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")
start = text.index("static XmFontList DefaultSystemFontList(")
end = text.index("\n/* XmeGetDefaultRenderTable", start)
region = text[start:end]

for needle in (
    "maxnsFontLists > INT_MAX - 8",
    "UINT_MAX / sizeof(SystemFontList)",
    "return NULL;",
    "maxnsFontLists += 8;",
):
    if needle not in region:
        raise SystemExit(f"default font-cache capacity guard missing: {needle}")

print("default font-list cache growth is bounded")
