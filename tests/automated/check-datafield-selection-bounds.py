#!/usr/bin/env python3
"""Check that DataField selection sizing is overflow-checked."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "if (num_chars > (size_t)UINT_MAX - 1)" in source
assert "capacity = num_chars + 1;" in source
assert "num_chars > SIZE_MAX / (size_t)XmTextF_max_char_size(tf) - 1" in source
assert "if (capacity > UINT_MAX)" in source
assert "value = XtMalloc((Cardinal)capacity);" in source

print("DataField selection bounds checks passed")
