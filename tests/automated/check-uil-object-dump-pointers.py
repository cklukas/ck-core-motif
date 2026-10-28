#!/usr/bin/env python3
"""Check the shared UIL object-header dump preserves pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
section = source[source.index("sym_dump_obj_header (XmConst") : source.index(
    "sym_dump_include_file (", source.index("sym_dump_obj_header (XmConst")
)]

assert '"%p %s  size: %d' in section
assert '"  name: %p"' in section
assert '"  reference: %p"' in section
assert '"  next: %p"' in section
assert "(void *)az_obj_entry" in section
assert "(unsigned)az_obj_entry" not in section
print("UIL object-header debug dumps preserve pointer widths")
