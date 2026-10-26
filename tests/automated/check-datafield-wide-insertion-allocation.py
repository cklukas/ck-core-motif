#!/usr/bin/env python3
"""Ensure DataField wide insertion allocation is failure-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (insert_length < 0", text.index("df_InsertString(") + 1)
end = text.index("\n\t      num_chars = mbstowcs", start)
block = text[start:end]

assert "insert_length < 0" in block
assert "SIZE_MAX / sizeof(wchar_t) - 1" in block
assert "UINT_MAX / sizeof(wchar_t)" in block
assert "XtMalloc(" in block
assert "if (wc_insert_string == NULL)" in block
assert "return;" in block

print("DataField wide insertion allocation guard: ok")
