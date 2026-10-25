#!/usr/bin/env python3
"""Check locale text aggregation sizing and allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmTextToLocaleText(")
end = source.index("\nvoid\n_XmConvertComplete", start)
region = source[start:end]

assert "size_t malloc_size = 0;" in region
assert "if (values[i] == NULL)" in region
assert "value_size > (size_t)-1 - malloc_size" in region
assert "if (malloc_size == (size_t)-1)" in region
assert "total_value = XtMalloc (malloc_size + 1);" in region
assert "if (total_value == NULL)" in region
assert region.count("XFreeStringList(values);") >= 4

print("PASS locale text aggregation checks bounds and allocation")
