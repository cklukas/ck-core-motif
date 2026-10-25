#!/usr/bin/env python3
"""Check that default render-table parsing visits all entries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(encoding="utf-8")
start = source.index("do {", source.index("XmeGetDefaultRenderTable"))
end = source.index("XtFree (newString);", start)
region = source[start:end]

assert "fontlist = XmFontListAppendEntry (fontlist, fontListEntry)" in region
assert "GetNextFontListEntry (&sPtr" in region
assert "!fontlist &&" not in region
print("PASS default render-table parsing preserves all entries")
