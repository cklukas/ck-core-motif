#!/usr/bin/env python3
"""Check UIL name-table lengths are validated before legacy-width storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymNam.c").read_text()

assert "#include <limits.h>" in source
assert source.count("(size_t)l_length > (size_t)USHRT_MAX - sym_k_name_entry_size - 1") == 2
assert "(unsigned short)(sym_k_name_entry_size +" in source
assert "az_new_name->header.b_type = (char)l_length" in source
assert "strncpy((char *)al_value, c_value, (size_t)l_length)" in source
print("UIL name lengths are bounded before allocation and compact storage")
