#!/usr/bin/env python3
"""Check that the DataField wide verify buffer is checked before conversion."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "int free_insert = (int)False;" in source
assert "if (temp == NULL) {" in source
assert "mod_ver_ret = False;" in source
assert "wcstombs(temp, XmTextF_wc_value(new_tf)" in source

print("DataField verify-buffer checks passed")
