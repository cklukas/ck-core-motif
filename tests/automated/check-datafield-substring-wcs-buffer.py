#!/usr/bin/env python3
"""Check wide DataField substring output validation."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "buffer == NULL || buf_size <= 0 || start < 0 || num_chars < 0" in source
assert "if (buf_size <= num_chars)" in source
assert "if (buf_size < num_chars + 1 )" not in source

print("DataField wide-substring buffer checks passed")
