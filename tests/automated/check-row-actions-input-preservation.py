#!/usr/bin/env python3
"""Check oversized RowActions input is rejected before existing items are freed."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("void XmRowActionsSetItems")
end = source.index("Cardinal XmRowActionsGetItemCount", start)
function = source[start:end]
guard = function.index("SIZE_MAX / sizeof(*actions->actions.items)")
free = function.index("free_items(actions, True)")
assert guard < free
assert "(count && !items)" in function
print("RowActions rejects oversized item arrays before replacing existing items")
