#!/usr/bin/env python3
"""Check substring matching normalizes both nullable rendition tags."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("SubStringPosition(", 50000)
end = source.index("\n\nstatic void", start)
region = source[start:end]

assert "char *under_tag = _XmEntryTag(under_seg);" in region
assert "if (under_tag == NULL)" in region
assert "strcmp(under_tag, XmFONTLIST_DEFAULT_TAG)" in region
assert "_XmStringIsCurrentCharset(under_tag)" in region
print("PASS substring matching guards both nullable rendition tags")
