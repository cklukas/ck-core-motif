#!/usr/bin/env python3
"""Check that Label wide-string conversion reserves its NUL byte."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nLabelSetValue")
region = source[start:source.index("static int\nLabelPreferredValue", start)]
assert "#include <stdint.h>" in source
assert "(SIZE_MAX - 1) / (size_t) MB_CUR_MAX" in region
assert "* (size_t) length + 1" in region
assert "wcstombs(str, str2, byte_length)" in region
print("Label wide-string conversion reserves a terminating byte")
