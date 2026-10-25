#!/usr/bin/env python3
"""Check font-list tag helpers reject a missing rendition tag safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmFontList.c").read_text()
entry_start = source.index("XmFontListEntryGetTag(")
entry_end = source.index("\n\nXmFontList \nXmFontListAppendEntry", entry_start)
entry_region = source[entry_start:entry_end]
assert "if (tag == NULL)" in entry_region

next_start = source.index("XmFontListGetNextFont(")
next_end = source.index("\n\nvoid\nXmFontListFreeFontContext", next_start)
next_region = source[next_start:next_end]
assert "if (tag == NULL)" in next_region
assert "XmRenditionFree(retained);" in next_region
print("PASS font-list tag helpers guard null rendition tags")
