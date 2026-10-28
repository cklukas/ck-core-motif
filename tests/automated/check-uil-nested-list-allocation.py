#!/usr/bin/env python3
"""Check UIL nested-list wrapper allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("void\t\tsar_add_list_entry")
end = source.index("void\t\tsar_add_forward_list_entry", start)
body = source[start:end]

assert body.count("if (nested_entry == NULL)") == 2
assert body.count("return;") >= 2
assert body.index("if (nested_entry == NULL)") < body.index("nested_entry->header.b_type")
assert body.index("if (nested_entry == NULL)", body.index("if (nested_entry == NULL)") + 1) < body.index("sym_make_value_forward_ref")
print("UIL nested-list allocations are guarded")
