#!/usr/bin/env python3
"""Check that XmString entry conversions fail without partial entries."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static _XmStringEntry \nEntryCvtToOpt(")
region = source[start:start + 2600]

assert "if (!new_entry)" in region
assert "if (len && !text)" in region

start = source.index("static _XmStringEntry \nEntryCvtToUnopt(")
region = source[start:start + 2600]
assert region.count("if (!new_entry)") == 1
assert region.count("_XmStringEntryFree(new_entry);") >= 3
assert "if (!_XmUnoptSegRendBegins(new_entry))" in region
assert "if (!_XmUnoptSegRendEnds(new_entry))" in region
print("PASS XmString entry conversions clean up allocation failures")
