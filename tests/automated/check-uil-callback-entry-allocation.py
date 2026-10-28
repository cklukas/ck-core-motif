#!/usr/bin/env python3
"""Check UIL callback-entry allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("void\t\tsar_save_reason_binding")
end = source.index("void\t\tsar_save_control", start)
body = source[start:end]
allocation = body.index("callback_entry =")
guard = body.index("if (callback_entry == NULL)", allocation)
assert guard < body.index("sym_make_value_forward_ref", guard)
assert guard < body.index("reason_frame->b_tag", guard)
print("UIL callback-entry allocation is guarded")
