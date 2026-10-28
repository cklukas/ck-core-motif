#!/usr/bin/env python3
"""Check DataField clear-selection spans before narrowing to int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("XmDataFieldWidget tf", source.index("df_ClearSelection(\n        Widget"))
end = source.index("static void", start)
body = source[start:end]

assert "XmTextPosition space_span" in body
assert "space_span > (XmTextPosition)(INT_MAX - 1)" in body
assert "num_spaces = (int)space_span" in body
assert "(size_t)num_spaces + 1 >" in body
assert "UINT_MAX / sizeof(wchar_t)" in body
print("DataField clear-selection span is bounds-checked")
