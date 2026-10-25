#!/usr/bin/env python3
"""Check XmFontListRemoveEntry rejects a missing tag before copying it."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmFontList.c").read_text()
start = source.index("XmFontListRemoveEntry(")
end = source.index("\nXmFontListEntry\nXmFontListEntryLoad", start)
region = source[start:end]

assert "if (tags[0] == NULL)" in region
assert region.index("if (tags[0] == NULL)") < region.index("tags[0] = XtNewString(tags[0]);")
print("PASS XmFontListRemoveEntry guards a null rendition tag")
