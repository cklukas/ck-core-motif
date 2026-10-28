#!/usr/bin/env python3
"""Check wide Text byte conversion reports an unrepresentable count as failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("converted = mbstowcs(wchar_t_ptr")
end = source.index("count = (int)converted", start)
body = source[start:end]
assert "converted == (size_t)-1" in body
assert "converted > (size_t)INT_MAX" in body
assert "return -1;" in body
print("PASS Text wide byte conversion reports count overflow as failure")
