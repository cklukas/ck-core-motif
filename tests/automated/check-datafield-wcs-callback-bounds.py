#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DataF.c").read_text()

needle = "if (*insert_length < 0 ||\n\t         (size_t)*insert_length + 1 >"
assert needle in source
assert "(size_t)UINT_MAX / sizeof(wchar_t)" in source
assert source.count("(unsigned) (*insert_length + 1)") == 0
