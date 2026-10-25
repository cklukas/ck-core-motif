#!/usr/bin/env python3
"""Check Xmos directory-entry list growth and string allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("_XmOSGetDirEntries(")
end = source.index("\n/****************************************************************\n * _XmOSBuildFileList", start)
region = source[start:end]

assert "String *new_entries;" in region
assert "unsigned new_alloc;" in region
assert "new_entries = (String *)XtRealloc" in region
assert "if (new_entries == NULL)" in region
assert "*pNumAlloc = new_alloc;" in region
assert "if (entryPtr == NULL)" in region
assert "replacement = XtMalloc(dirNameLen + 1)" in region
assert "if (replacement == NULL)" in region

print("PASS Xmos directory-entry list allocation checks")
