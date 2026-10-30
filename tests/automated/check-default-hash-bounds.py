#!/usr/bin/env python3
"""Check the default Motif hash bounds pointer values portably."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text(encoding="utf-8")
start = source.index("Hash(XmHashKey x)")
end = source.index("/* Hash callbacks", start)
region = source[start:end]
assert "(uintptr_t)x % (uintptr_t)INT_MAX" in region
assert "(long) x" not in region
print("PASS default Motif hash bounds pointer values before conversion")
