#!/usr/bin/env python3
"""Check XPM hash growth publishes metadata after allocation succeeds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmhashtab.c").read_text()
start = source.index("HashTableGrows(")
start = source.index("{", start)
end = source.index("\n}\n", start) + 2
function = source[start:end]
alloc = function.index("atomTable = (xpmHashAtom *) XpmMalloc")
publish = function.index("table->size = size")
assert alloc < publish
assert "table->limit = size / 3" in function
print("PASS XPM hash growth failure state")
