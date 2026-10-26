#!/usr/bin/env python3
"""Check DataField drop-transfer length conversions are bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_DropTransferCallback")
body = source[start:source.index("if (replace)", start)]
assert "size_t malloc_size" in body
assert "malloc_size >= (size_t)UINT_MAX" in body
assert "total_length > (size_t)INT_MAX" in body
assert "total_length + 1 > (size_t)UINT_MAX / sizeof(wchar_t)" in body
assert "mbstowcs(wc_total_tmp_value, total_tmp_value" in body
assert "total_length = mbstowcs" not in body
assert "mbstowcs(wc_total_tmp_value, total_tmp_value,\n\t\t\t\t       (int)total_length)" not in body
print("PASS DataField drop-transfer lengths are bounded")
