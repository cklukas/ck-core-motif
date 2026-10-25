#!/usr/bin/env python3
"""Check Unoptimize frees an array entry on size validation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("Unoptimize(")
end = source.index("\n\n\n/****************************************************************", start)
region = source[start:end]

assert "segment_count * sizeof(*segments)" in region
assert "_XmStringEntryFree(new_entry);" in region
overflow = region.index("segment_count * sizeof(*segments)")
assert region.index("_XmStringEntryFree(new_entry);", overflow) < region.index("return NULL;", overflow)
print("PASS Unoptimize frees its entry on size overflow")
