#!/usr/bin/env python3
"""Check DataField modify-verify result buffers are checked before use."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("if (XmTextF_wcs_modify_verify_callback(tf))")
end = source.index("static void", start)
body = source[start:end]

assert body.count("if (*insert == NULL)") >= 3
assert "if (vcb.text == NULL)" in body
assert "(size_t)*insert_length >= (size_t)UINT_MAX" in body
assert "XtMalloc((unsigned)(*insert_length + 1)" not in body
assert "(Cardinal)(((size_t)*insert_length + 1) *" in body
assert body.count("goto modify_verify_allocation_failed;") >= 2
assert "modify_verify_allocation_failed:" in body
assert "XtFree((char*)wcs_newblock.wcsptr);" in body
print("DataField modify-verify result allocations are checked")
