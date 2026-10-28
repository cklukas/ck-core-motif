#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DataF.c").read_text()

assert "if (*insert_length < 0 ||" in source
assert "wcs_vcb.text->wcsptr == NULL" in source
assert "(size_t)UINT_MAX / sizeof(wchar_t)" in source
assert source.count("(unsigned) (*insert_length + 1)") == 0
