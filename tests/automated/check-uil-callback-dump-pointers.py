#!/usr/bin/env python3
"""Check UIL callback dumps preserve pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
start = source.rindex("sym_dump_callback")
section = source[start : source.index("sym_dump_list", start)]

assert '"  reason name: %p  proc ref: %p  proc ref list: %p\\n"' in section
for field in ("az_call_reason_name", "az_call_proc_ref", "az_call_proc_ref_list"):
    assert f"(void *)az_callback_entry->{field}" in section
print("UIL callback dumps preserve pointer widths")
