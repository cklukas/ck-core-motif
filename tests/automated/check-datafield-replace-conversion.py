#!/usr/bin/env python3
"""Check DataField replace conversion ownership and allocation guards."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "value_length + 1 > SIZE_MAX / sizeof(wchar_t)" in source
assert "if (wc_value == NULL)" in source
assert "if (wc_length == (size_t) -1)" in source
assert "tmp[0] = '\\0';" in source
assert 'tmp = ""' not in source
assert "if (tmp == NULL)" in source

print("DataField replace-conversion checks passed")
