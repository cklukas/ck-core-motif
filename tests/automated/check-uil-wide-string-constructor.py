#!/usr/bin/env python3
"""Check UIL wide-string construction handles node allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarComp.c").read_text()
start = source.index("sem_create_wchar_str()")
start = source.rfind("sym_value_entry_type", 0, start)
end = source.index("\n\f\n/*", start)
body = source[start:end]

assert "if (az_wchar_str_entry == NULL)" in body
assert body.index("if (az_wchar_str_entry == NULL)") < body.index("az_wchar_str_entry->obj_header")
print("UIL wide-string constructor checks node allocation")
