#!/usr/bin/env python3
"""Check that name-less format enumeration avoids null atom lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static ClipboardFormatItem \nClipboardFindFormat(")
end = source.index("static void \nClipboardDeleteFormat(", start)
region = source[start:end]

assert "formatatom = None;" in region
assert "if (format != NULL)" in region
assert "formatatom = XInternAtom( display, format, False );" in region
assert region.index("format != NULL") < region.index("formatatom = XInternAtom")
print("PASS name-less clipboard format enumeration avoids null atom lookup")
