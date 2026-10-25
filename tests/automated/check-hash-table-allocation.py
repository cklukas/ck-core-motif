#!/usr/bin/env python3
"""Check shared hash-table allocation handles the table allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
start = source.index("_XmAllocHashTable(Cardinal size_hint")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "table = (XmHashTable) XtMalloc" in region
assert "if (table == NULL)" in region
assert region.index("if (table == NULL)") < region.index("table -> hasher")
assert "table->buckets == NULL" in region
print("PASS hash-table allocation guard")
