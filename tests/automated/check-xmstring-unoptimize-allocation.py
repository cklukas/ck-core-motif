#!/usr/bin/env python3
"""Check that non-destructive XmString unoptimization cleans up failures."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static _XmStringEntry\nUnoptimize(")
end = source.index("\n\n\nXmString", start)
region = source[start:end]

assert "segment_count > SIZE_MAX / sizeof(*segments)" in region
assert "if (!segments)" in region
assert "if (!segments[j])" in region
assert "_XmStringEntryFree((_XmStringEntry)segments[k]);" in region
print("PASS XmString non-destructive unoptimization cleans up failures")
