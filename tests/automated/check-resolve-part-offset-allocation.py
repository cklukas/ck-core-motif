#!/usr/bin/env python3
"""Check the public part-offset resolver validates inputs and allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Resolve.c").read_text(encoding="utf-8")
start = source.index("XmeResolvePartOffsets(")
end = source.index("\n\n\nvoid \nXmResolveAllPartOffsets", start)
region = source[start:end]

assert "if (w_class == NULL || offset == NULL)" in region
assert "classcount <= 0" in region
assert "classcount > UINT_MAX / sizeof(XmOffset)" not in region
assert "(size_t)classcount > (size_t)UINT_MAX / sizeof(XmOffset)" in region
assert "if (*offset == NULL)" in region
assert "if (constraint_offset != NULL && *constraint_offset == NULL)" in region
print("PASS public part-offset resolution validates allocation state")
