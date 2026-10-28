#!/usr/bin/env python3
"""Check UIL compound-string lengths before compact-record narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "int\t\t\tcstr_length;" in source
assert "cstr_length < 0 || cstr_length > USHRT_MAX" in source
assert "csval_entry->w_length = (unsigned short)cstr_length;" in source
assert "csval_entry->w_length = XmStringLength (cstr_r);" not in source
print("UIL compound-string lengths are checked before narrowing")
