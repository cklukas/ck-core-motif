#!/usr/bin/env python3
"""Check that non-DnD context owners use integer zero for XContext null state."""
from pathlib import Path
import sys

if len(sys.argv) < 2:
    root = Path(__file__).parents[2]
    names = sorted((root / "lib" / "Xm").glob("*.c"))
else:
    names = [Path(name) for name in sys.argv[1:]]
for name in names:
    text = Path(name).read_text(encoding="utf-8")
    assert "(XContext) NULL" not in text, f"legacy XContext null cast remains in {name}"
print("PASS XContext null state uses an integer zero")
