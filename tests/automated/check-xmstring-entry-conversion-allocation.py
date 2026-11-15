#!/usr/bin/env python3
"""Check that XmString entry conversions fail without partial entries."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static _XmStringEntry \nEntryCvtToOpt(")
end = source.index("static _XmStringEntry \nEntryCvtToUnopt(", start)
region = source[start:end]

assert "if (!new_entry)" in region
assert "if (len && !text)" in region

start = end
region = source[start:source.index("/* Convert an optimized _XmString", start)]
assert region.count("if (!new_entry)") == 1
assert region.count("_XmStringEntryFree(new_entry);") >= 3
assert "if (!_XmUnoptSegRendBegins(new_entry))" in region
assert "if (!_XmUnoptSegRendEnds(new_entry))" in region
print("PASS XmString entry conversions clean up allocation failures")
