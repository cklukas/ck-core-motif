#!/usr/bin/env python3
"""Check UIL cycle validation tests the mask as a mask, not as literal 1."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "return (cycle_name->b_flags & sym_m_has_cycle) != 0;" in source
assert "(cycle_name->b_flags&sym_m_has_cycle) == 1" not in source
print("UIL cycle validation correctly tests the cycle bit mask")
