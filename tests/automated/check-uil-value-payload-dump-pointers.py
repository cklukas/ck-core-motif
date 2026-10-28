#!/usr/bin/env python3
"""Check UIL value-payload diagnostics preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_value")
section = source[start : source.index("sym_dump_source_info", start)]

for marker in (
    'first component: %p',
    'next table entry: %p',
    'userdefined(%p)',
    'colors: %p  rows: %p',
    'first table entry: %p',
    'color: %p',
):
    assert marker in section, marker
for expression in (
    "az_value_entry->az_first_table_value",
    "az_value_entry->az_next_table_value",
    "az_value_entry->az_charset_value",
    "az_value_entry->value.z_icon->az_color_table",
    "az_value_entry->value.z_icon->az_rows",
    "az_value_entry->value.z_color[index].az_color",
):
    assert f"(void *){expression}" in section, expression
print("UIL value-payload dumps preserve pointer widths")
