#!/usr/bin/env python3
"""Check DataField string conversion sizing and allocation guards."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "capacity = (size_t)(XmTextF_string_length(tf) + 1)" in source
assert "if (capacity > UINT_MAX)" in source
assert "if (temp_str == NULL)" in source
assert "if (wc_value == NULL)" in source
assert "if (tmp == NULL)" in source

print("DataField string-conversion checks passed")
