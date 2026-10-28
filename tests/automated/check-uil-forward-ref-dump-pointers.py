#!/usr/bin/env python3
"""Check UIL forward-reference dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_forward_ref")
section = source[start : source.index("sym_dump_value", start)]

assert '"%p forward ref  size: %d  next ref: %p  location: %p' in section
assert '"  name: %p %s\\n"' in section
for field in ("az_forward_ref_entry", "az_next_ref", "a_update_location", "parent", "az_name"):
    assert f"(void *)az_forward_ref_entry->{field}" in section or field == "az_forward_ref_entry"
assert "(void *)az_forward_ref_entry" in section
print("UIL forward-reference dumps preserve pointer widths")
