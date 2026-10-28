#!/usr/bin/env python3
"""Check UIL enum codes are explicitly stored in their 16-bit field."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
assert "value_entry->b_enumeration_value_code =\n\t\t    (unsigned short)enum_code;" in source
assert "value_entry->b_enumeration_value_code = enum_code;" not in source
print("UIL enumeration-code narrowing is explicit")
