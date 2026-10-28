#!/usr/bin/env python3
"""Check UIL forward-list wrapper allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("void\t\tsar_add_forward_list_entry")
end = source.index("\f", start)
body = source[start:end]
allocation = body.index("nested_entry =")
guard = body.index("if (nested_entry == NULL)", allocation)
assert guard < body.index("sym_make_value_forward_ref", guard)
assert guard < body.index("entry_entry =", guard)
print("UIL forward-list allocation is guarded")
