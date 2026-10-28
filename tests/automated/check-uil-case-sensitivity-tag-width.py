#!/usr/bin/env python3
"""Check UIL case-sensitivity metadata uses an explicit tag-width cast."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarMod.c").read_text()
assert "sym_az_module_entry->az_case_sense->header.b_type =\n\t(unsigned char)uil_v_case_sensitive;" in source
print("UIL case-sensitivity tag narrowing is explicit")
