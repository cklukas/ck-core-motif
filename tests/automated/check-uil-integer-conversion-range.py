#!/usr/bin/env python3
"""Check UIL numeric-to-int conversion rejects unrepresentable values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()
section = source[source.index("int\tsem_convert_to_integer") :]

assert "value.l_integer < INT_MIN" in section
assert "value.l_integer > INT_MAX" in section
assert "value.d_real != operand_entry->value.d_real" in section
assert "value.d_real < (double)INT_MIN" in section
assert "value.single_float !=" in section
assert "res_type = error_arg_type;" in section
print("UIL numeric-to-integer conversion rejects overflow and NaN")
