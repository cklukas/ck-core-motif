#!/usr/bin/env python3
"""Check that XmString separator creation cleans up partial allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmString \nXmStringSeparatorCreate(")
end = source.index("\n/*\n * copy a refcounted", start)
region = source[start:end]

assert "entries = (_XmStringEntry *)XtMalloc" in region
assert "if (!entries)" in region
assert "if (!entries[i])" in region
assert "_XmStringEntryFree(entries[j]);" in region
assert region.count("return NULL;") >= 2
print("PASS XmString separator creation guards partial allocations")
