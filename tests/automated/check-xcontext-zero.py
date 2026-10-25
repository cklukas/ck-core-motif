#!/usr/bin/env python3
"""Check that non-DnD context owners use integer zero for XContext null state."""
from pathlib import Path
import sys

if len(sys.argv) < 2:
    raise SystemExit(f"usage: {sys.argv[0]} SOURCE...")
for name in sys.argv[1:]:
    text = Path(name).read_text(encoding="utf-8")
    assert "(XContext) NULL" not in text, f"legacy XContext null cast remains in {name}"
print("PASS XContext null state uses an integer zero")
