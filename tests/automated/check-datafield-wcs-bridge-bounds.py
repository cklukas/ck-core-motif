#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DataF.c").read_text()

assert "vcb.text->length < 0" in source
assert "(size_t)vcb.text->length + 1 >" in source
assert "(size_t)*insert_length + 1 >" in source
assert "(size_t)UINT_MAX / sizeof(wchar_t)" in source
assert "(vcb.text->length + 1) * sizeof(wchar_t)" not in source
assert "(*insert_length + 1) * sizeof(wchar_t)" not in source
