#!/usr/bin/env python3
"""Check UIL temporary color-item construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
start = source.index("void\tsar_make_color_item")
end = source.index("void\tsar_append_color_item", start)
body = source[start:end]

assert "if (item_entry == NULL)" in body
assert "target_frame->b_tag = sar_k_null_frame;" in body
assert "sem_free_node ((sym_entry_type *)item_entry);" in body
assert body.index("if (item_entry == NULL)") < body.index("_sar_save_source_pos (&item_entry->header")
print("UIL color-item allocation and cleanup are guarded")
