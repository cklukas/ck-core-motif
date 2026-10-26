#!/usr/bin/env python3
"""Ensure overstrike preedit concatenation handles source and realloc failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("  if (mb == NULL)")
end = text.index("\n    if (recover_len > 0)", start)
block = text[start:end]

assert "if (mb == NULL)" in block
assert "if (over_mb == NULL)" in block
assert "SIZE_MAX - mb_length - 1" in block
assert "resized_mb = XtRealloc" in block
assert "if (resized_mb == NULL)" in block
assert "memcpy(mb + mb_length, over_mb, over_length + 1)" in block

print("text preedit overstrike concatenation guards: ok")
