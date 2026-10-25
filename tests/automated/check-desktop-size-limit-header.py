#!/usr/bin/env python3
"""Check that Desktop includes the header defining UINT_MAX."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Desktop.c"
text = source.read_text(encoding="utf-8")
assert "#include <limits.h>" in text
assert "UINT_MAX" in text
print("desktop size-limit header present")
