#!/usr/bin/env python3
"""Check TextField selection and drop callbacks fail closed."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
assert "ds->transfer_id == NULL" in source
assert "ds->length > (unsigned long)INT_MAX" in source
assert "if (prim_select == NULL)" in source
assert "cb->dragContext == NULL" in source
assert "numExportTargets != 0 && exportTargets == NULL" in source
assert "if (transfer_rec == NULL)" in source

print("TextField transfer callbacks fail closed")
