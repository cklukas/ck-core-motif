#!/usr/bin/env python3
"""Ensure text SetValues wide-string conversion handles storage failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (newtw->text.wc_value != NULL)")
end = text.index("\n  } else if (newtw->text.value != NULL)", start)
block = text[start:end]

assert "num_chars == INT_MAX" in block
assert "UINT_MAX / (size_t)newtw->text.char_size" in block
assert "tmp_value = XtMalloc((Cardinal)allocation_bytes);" in block
assert "if (tmp_value != NULL)" in block
assert "n_bytes != (size_t)-1" in block

print("text SetValues wide-string guard: ok")
