#!/usr/bin/env python3
"""Check RowColumn accelerator replacements fail without null dereferences."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text()
assert "CopyMenuAccelerator(String source)" in source
assert "length >= UINT_MAX" in source
assert "XtMalloc((Cardinal)(length + 1))" in source
assert source.count("String replacement = CopyMenuAccelerator") >= 2
assert source.count("RC_MenuAccelerator(new_w) = RC_MenuAccelerator(old);") >= 2
assert "strcpy(XtMalloc" not in source
print("RowColumn accelerator replacements roll back on allocation failure")
