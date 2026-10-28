#!/usr/bin/env python3
"""Check UIL parser enum-to-legacy-field boundaries are explicit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarMod.c").read_text()
start = source.index("void\tsar_process_module_variant")
end = source.index("void\tsar_save_section_source", start)
variant = source[start:end]
section = source[end:source.index("\n}\n", end)]

assert "diag_object_text ((int)obj_type)" in variant
assert "diag_tag_text ((int)obj_variant)" in variant
assert "uil_urm_variant[obj_type] = (unsigned short)obj_variant;" in variant
assert "b_object_info = (char)obj_type;" in variant
assert "b_variant_info = (char)obj_variant;" in variant
assert "section_entry->header.b_type = (unsigned char)section_type;" in section
print("UIL enum narrowing boundaries are explicit")
