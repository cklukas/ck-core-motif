#!/usr/bin/env python3
"""Check UIL value-frame compact fields narrow explicitly at their boundaries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()

assert "value_entry->b_type = (unsigned char)value_type;" in source
assert "value_frame->b_type = (unsigned char)value_type;" in source
assert "value_frame->b_flags = (unsigned short)value_entry->obj_header.b_flags;" in source
assert "operator_frame->b_type = (unsigned char)target_type;" in source
assert "target_frame->b_charset = (unsigned char)" in source
assert "cstr_entry->b_charset = (unsigned char)" in source
print("UIL compact value-frame fields use explicit, value-preserving narrowing")
