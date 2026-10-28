#!/usr/bin/env python3
"""Ensure UIL byte tables reject counts larger than their allocated record."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text(
    encoding="utf-8"
)
start = text.index("switch (header.table_id)")
end = text.index("\n\t    case Key_Table:", start)
region = text[start:end]
assert region.count("header.num_items > (size_t)header.table_size") == 6
assert region.count("d_bad_database") == 6
assert "fread (constraint_tab" in region
assert "fread (argument_type_table" in region
print("UIL byte-table reads are bounded by their allocations")
