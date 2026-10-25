#!/usr/bin/env python3
"""Check that non-optimized XmString clones clean up partial copies."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static XmString \nClone(")
end = source.index("Given a string in ASN.1", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(*entries)" in region
assert "if (entries == NULL)" in region
assert "_XmStringEntryCopy(_XmStrEntry(string)[i])" in region
assert "_XmStringEntryFree(entries[j]);" in region
assert "return NULL;" in region
print("PASS XmString clone allocations clean up partial copies")
