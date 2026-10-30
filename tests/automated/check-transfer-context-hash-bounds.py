#!/usr/bin/env python3
"""Check transfer context hashing avoids signed pointer arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
start = source.index("CCHash(XtPointer x)")
end = source.index("static XmHashTable ConvertHashTable", start)
region = source[start:end]
assert "uintptr_t mixed;" in region
assert "mixed = (uintptr_t)a->display" in region
assert "mixed % (uintptr_t)INT_MAX" in region
assert "(long) a -> display + (long) a -> selection" not in region
print("PASS transfer context hash uses bounded uintptr_t mixing")
