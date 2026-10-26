#!/usr/bin/env python3
"""Check narrow DataField substring validation precedes pointer arithmetic."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "if (buffer == NULL || start < 0 || num_chars < 0 ||" in source
assert "num_chars > XmTextF_string_length(tf) - start" in source
assert "if (n_bytes < 0 || n_bytes >= INT_MAX || buf_size <= n_bytes)" in source

print("DataField byte-substring range checks passed")
