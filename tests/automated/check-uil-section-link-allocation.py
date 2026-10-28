#!/usr/bin/env python3
"""Check UIL section-list links handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()

for function, next_function in (
    ("void\tsar_bind_value_name", "void\tsar_create_identifier"),
    ("void\tsar_create_identifier", "void\tsar_make_font_table"),
):
    start = source.index(function)
    end = source.index(next_function, start)
    body = source[start:end]
    allocation = body.index("section_entry = (sym_section_entry_type *) sem_allocate_node")
    guard = body.index("if (section_entry == NULL)", allocation)
    link = body.index("section_entry->next", guard)
    assert guard < link
print("UIL section-link allocations are guarded")
