#!/usr/bin/env python3
"""Check Xmos qualified-path sizing before narrowing or allocating."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("filterLenSize = strlen(filterSpec)")
body = source[start:start + 1200]
assert "filterLenSize = strlen(filterSpec)" in body
assert "dirLenSize = strlen(dirSpec)" in body
assert "filterLenSize > (size_t)INT_MAX - 2" in body
assert "filterLenSize + dirLenSize + 4 > (size_t)UINT_MAX" in body
assert "XtMalloc((Cardinal)(filterLenSize + dirLenSize + 4))" in body
print("PASS qualified directory length bounds")
