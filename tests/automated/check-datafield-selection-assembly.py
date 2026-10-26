#!/usr/bin/env python3
"""Check DataField selection assembly handles allocation failure safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataFSel.c").read_text(
    encoding="utf-8"
)

assert "total_tmp_value == NULL" in source
assert "malloc_size + 1 > UINT_MAX / sizeof(wchar_t)" in source
assert "wc_value == NULL" in source
assert "*length > (unsigned long)INT_MAX" in source
assert "temp == NULL" in source
print("DataField selection assembly checks temporary allocations")
