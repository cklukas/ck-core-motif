#!/usr/bin/env python3
"""Check RowActions item construction fails transactionally."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/RowActions.c"
text = source.read_text(encoding="utf-8")

assert "if (!combined)" in text
start = text.index("void XmRowActionsSetItems")
body = text[start:text.index("Cardinal XmRowActionsGetItemCount", start)]
assert "if (!actions->actions.items)" in body
assert "actions->actions.item_count = i + 1;" in body
assert body.count("free_items(actions, True);") >= 4
assert "if (!actions->actions.items[i].button)" in body
print("RowActions item construction is transactional")
