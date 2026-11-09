#!/usr/bin/env python3
"""Check RowActions item construction fails transactionally."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/RowActions.c"
text = source.read_text(encoding="utf-8")

start = text.index("void XmRowActionsSetItems")
body = text[start:text.index("Cardinal XmRowActionsGetItemCount", start)]
assert "if (!replacement)" in body
assert "free_item_array(replacement, i + 1, True);" in body
assert body.index("free_item_array(replacement, i + 1, True);") < body.index(
    "free_items(actions, True);"
)
assert "actions->actions.items = replacement;" in body
assert "XtManageChild(actions->actions.items[i].button);" in body
print("RowActions item construction is transactional")
