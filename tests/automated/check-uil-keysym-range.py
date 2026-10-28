#!/usr/bin/env python3
"""Check private integer-to-keysym conversion stays within one byte."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "unsigned char tmp;" in source
assert "value.l_integer >= 0" in source
assert "value.l_integer <= UCHAR_MAX" in source
assert "tmp = (unsigned char)argument_entry->az_arg_value->value.l_integer;" in source
print("UIL integer-to-keysym conversion rejects values outside one byte")
