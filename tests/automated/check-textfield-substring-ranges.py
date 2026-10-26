#!/usr/bin/env python3
"""Ensure TextField substring APIs validate ranges before arithmetic or access."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)

for name in ("XmTextFieldGetSubstring(", "XmTextFieldGetSubstringWcs("):
    start = source.index(name)
    end = source.index("\n\n\n", start)
    region = source[start:end]
    assert "buffer == NULL" in region
    assert "start < 0 || start > tf->text.string_length" in region
    assert "buf_size <= 0" in region
    assert "start + num_chars" not in region

assert "buf_size <= n_bytes" in source
assert "buf_size <= num_chars" in source

print("TextField substring APIs validate buffers, ranges, and capacities")
