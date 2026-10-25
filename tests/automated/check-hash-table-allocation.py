#!/usr/bin/env python3
"""Check hash-table record allocation is validated before initialization."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
start = source.index("_XmAllocHashTable(")
start = source.rfind("\n", 0, start) + 1
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "table = (XmHashTable) XtMalloc(sizeof(XmHashTableRec));" in region
assert "if (table == NULL)" in region
assert region.index("if (table == NULL)") < region.index("table -> hasher")
assert "return (XmHashTable) NULL;" in region
print("PASS hash-table record allocation guard")
