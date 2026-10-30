#!/usr/bin/env python3
"""Ensure compact XmStrings are never probed as padded entry records."""

from pathlib import Path

root = Path(__file__).parents[2]
source = (root / "lib" / "Xm" / "XmString.c").read_text()
header = (root / "lib" / "Xm" / "XmStringI.h").read_text()

assert "_XmStrOptTextNeedsPadding" in header
assert "offsetof(_XmStringOptSegRec, data) != sizeof(_XmStringOptHeader)" in header
assert "_XmStrText(b) != (char *)_XmEntryTextGet((_XmStringEntry)b)" not in source
assert "_XmStrText(*underline) !=" not in source
print("PASS compact XmString layout checks avoid out-of-bounds entry probes")
