#!/usr/bin/env python3
"""Ensure overstrike recovery growth checks sizing and XtRealloc."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (recovery_valid) {")
end = text.index("\n      }\n    }\n  }", start)
block = text[start:end]

assert "total_mb_len > SIZE_MAX - mb_length - 1" in block
assert "SIZE_MAX - mb_length - 1" in block
assert "resized_mb = XtRealloc" in block
assert "if (resized_mb == NULL)" in block
assert "memcpy(mb + mb_length, ptr" in block

print("text preedit recovery growth guard: ok")
