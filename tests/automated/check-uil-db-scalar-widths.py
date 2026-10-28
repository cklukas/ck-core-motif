#!/usr/bin/env python3
"""Check UIL database scalar fields cross legacy widths explicitly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()

assert "return_num_items = (int)fread (&globals" in source
assert "charset_lang_table_max = (unsigned short)globals.charset_lang_table_max" in source
assert "uil_sym_user_defined_object = (unsigned short)i" in source
print("UIL database scalar width boundaries are explicit")
