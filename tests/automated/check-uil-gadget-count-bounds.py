#!/usr/bin/env python3
"""Check UIL gadget counts are bounded before entering compact records."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "gadget_count < 0 || gadget_count > USHRT_MAX" in source
assert "list_entry->w_gadget_count = (unsigned short)gadget_count;" in source
assert "list_entry->w_gadget_count = gadget_count;" not in source
print("UIL gadget counts are checked before narrowing to the symbol-record width")
