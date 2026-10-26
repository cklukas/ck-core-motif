#!/usr/bin/env python3
"""Ensure overstrike preedit storage checks size and allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (PreOverMaxLen(tw) == PreOverLen(tw))")
end = text.index("\n    if (need_verify)", start)
block = text[start:end]

assert "PreOverMaxLen(tw) < 0" in block
assert "UINT_MAX / (size_t)tw->text.char_size" in block
assert "if (mb == NULL)" in block
assert "ResetUnder(tw);" in block

print("text preedit overstrike allocation guard: ok")
