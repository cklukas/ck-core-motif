#!/usr/bin/env python3
"""Ensure RowActions replacement construction preserves current actions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmRowActionsSetItems")
end = source.index("\nCardinal XmRowActionsGetItemCount", start)
body = source[start:end]

assert "XmRowActionsStoredItem *replacement = NULL" in body
assert "static void free_item_array" in source
assert "free_item_array(replacement, i + 1, True);" in body
assert body.index("if (!replacement)") < body.index("free_items(actions, True);")
assert "actions->actions.items = replacement;" in body
print("PASS RowActions replacement is transactional")
