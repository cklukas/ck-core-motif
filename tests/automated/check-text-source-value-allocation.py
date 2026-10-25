#!/usr/bin/env python3
"""Check Text source value conversion sizing and failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmStringSourceGetValue(")
end = source.index("\nvoid \n_XmStringSourceSetValue", start)
region = source[start:end]
assert region.count("TextSourceRangeBytes(0, data->length") == 2
assert region.count("if (.* == NULL") == 0  # guard spelling checked below
assert "if (temp == NULL)" in region
assert "if (wc_temp == NULL)" in region
assert "XtFree((char *)wc_temp);" in region
assert "(int)(ret_pos - pos)" not in region
assert "(size_t)(ret_pos - pos)" in region
print("PASS Text source value conversion checks ranges and allocations")
