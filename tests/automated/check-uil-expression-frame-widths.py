#!/usr/bin/env python3
"""Check UIL expression frames narrow known operator and flag fields explicitly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarExp.c").read_text()

assert "res_entry->b_expr_opr = (unsigned char)operator;" in source
assert "res_entry->b_type = (unsigned char)operator;" in source
assert "res_entry->b_type = (unsigned char)res_type;" in source
assert source.count("operator_frame->b_flags = (unsigned short)res_entry->obj_header.b_flags;") == 2
print("UIL expression frame operator and flag widths are explicit")
