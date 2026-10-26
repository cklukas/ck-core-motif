#!/usr/bin/env python3
"""Ensure existing-source XmNwcValue initialization checks conversion storage."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (tw->text.wc_value != NULL)")
end = text.index("\n    } else if (tw->text.value != NULL)", start)
block = text[start:end]

assert "num_chars != INT_MAX" in block
assert "UINT_MAX / (size_t)tw->text.char_size" in block
assert "tmp_value = XtMalloc((Cardinal)allocation_bytes);" in block
assert "if (tmp_value != NULL)" in block
assert "if (n_bytes >= 0)" in block

print("text initialization wide-string guard: ok")
