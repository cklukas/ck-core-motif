#!/usr/bin/env python3
"""Check UIL expression type codes cross their compact fields explicitly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "op1_type = (char)validate_arg" in source
assert "op2_type = (char)validate_arg" in source
assert "op1_type = (char)(* numeric_convert_table" in source
assert "op2_type = (char)(* numeric_convert_table" in source
assert "res_type = (char)sem_convert_to_integer" in source
assert "res_type = (char)sem_convert_to_float" in source
assert "res_type = (char)sem_convert_to_single_float" in source
assert "&= ~(unsigned int)sym_m_forward_ref" in source
print("UIL expression type and flag widths are explicit at compact fields")
