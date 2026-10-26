#!/usr/bin/env python3
"""Check RowActions item-array allocation bounds count arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("void XmRowActionsSetItems")
body = source[start:source.index("Cardinal XmRowActionsGetItemCount", start)]
assert "SIZE_MAX / sizeof(*actions->actions.items)" in body
assert "UINT_MAX / sizeof(*actions->actions.items)" in body
assert "XtCalloc(count, sizeof(*actions->actions.items))" in body
assert "item array is too large" in body
print("PASS RowActions item-array allocation bounds")
