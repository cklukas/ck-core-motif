#!/usr/bin/env python3
"""Check UIL compound-string append operations handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarComp.c").read_text()

append_start = source.index("void\tsem_append_str_to_cstr")
append = source[append_start:source.index("\f\n/*", append_start)]
cat_start = source.index("*sem_cat_str_to_str")
cat = source[cat_start:source.index("\f\n/*", cat_start)]
merge_start = source.index("void\tsem_append_cstr_to_cstr")
merge = source[merge_start:source.index("\f\n/*", merge_start)]

assert "if (az_cstr_entry == NULL || az_str_entry == NULL)" in append
assert "if (new_str_entry == NULL)" in append
assert "if (new_str_entry == NULL)" in cat
assert "if (new_str_entry->value.c_value == NULL)" in cat
assert "sem_free_node ((sym_entry_type *)new_str_entry);" in cat
assert "if (az_cstr1_entry == NULL || az_cstr2_entry == NULL)" in merge
print("UIL compound-string append allocations are guarded")
