#!/usr/bin/env python3
"""Check UIL integer-to-keysym coercion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()
start = source.index("void sem_validate_argument_entry")
end = source.index("void sem_validate_argument_enumset", start)
body = source[start:end]
allocation = body.index("keysym_value = (char *) XtCalloc(1,2)")
guard = body.index("if (keysym_value == NULL)", allocation)
copy = body.index("_move (argument_entry->az_arg_value->value.c_value", guard)
assert guard < copy
assert body.index("valid_value = TRUE", guard) < copy
print("UIL keysym coercion allocation is guarded")
