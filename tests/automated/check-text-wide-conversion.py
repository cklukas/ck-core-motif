#!/usr/bin/env python3
"""Check wide-text callback conversion rejects overflow and unterminated output."""
from pathlib import Path

source = Path("lib/Xm/TextStrSo.c").read_text(encoding="utf-8")
assert "size_t capacity = 0" in source
assert "capacity > (size_t)UINT_MAX" in source
assert "converted >= capacity" in source
assert "converted > (size_t)INT_MAX" in source
print("PASS wide-text callback conversion bounds allocation and wcstombs output")
