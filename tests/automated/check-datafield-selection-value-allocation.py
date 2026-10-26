#!/usr/bin/env python3
"""Check DataField text selection values use bounded allocations."""
from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
assert "datafield_selection_alloc(unsigned long length)" in source
assert "if (length >= (unsigned long)UINT_MAX)" in source
assert source.count("datafield_selection_alloc(*length)") >= 6

for marker in ("tmp_value = datafield_selection_alloc(*length);", "*value = (XtPointer)datafield_selection_alloc(*length);"):
    offset = 0
    while True:
        try:
            offset = source.index(marker, offset)
        except ValueError:
            break
        following = source[offset : offset + 180]
        assert "== NULL" in following
        offset += len(marker)

assert "XtMalloc((unsigned) *length + 1)" not in source
print("PASS DataField selection values use bounded allocation checks")
