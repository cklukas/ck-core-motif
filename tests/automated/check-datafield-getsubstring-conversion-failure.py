#!/usr/bin/env python3
"""Ensure DataField substring conversion errors reach callers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text()
start = source.index("XmDataFieldGetSubstring(")
end = source.index("\n\nwchar_t *", start)
body = source[start:end]
assert "if (wcs_ret == (size_t)-1) {" in body
assert "ret_value = XmCOPY_FAILED;" in body
assert body.index("ret_value = XmCOPY_FAILED;") > body.index(
    "wcs_ret = wcstombs"
)
print("DataField substring conversion failures are reported")
