#!/usr/bin/env python3
"""Ensure toolbar replacement allocation cannot erase the current actions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmToolbarSetActions")
end = source.index("\nvoid XmToolbarSetActionIcons", start)
body = source[start:end]

assert "XmToolbarStoredAction *replacement = NULL;" in body
assert "static void free_action_array" in source
assert "replacement = (XmToolbarStoredAction *)XtCalloc" in body
assert "if (replacement == NULL)" in body
assert "free_action_array(replacement, count, True);" in body
assert body.index("if (replacement == NULL)") < body.index("free_actions(toolbar, True);")
assert body.index("stored = &replacement[i]") < body.index("free_actions(toolbar, True);")
assert "toolbar->toolbar.actions = replacement;" in body
print("PASS Toolbar action replacement preserves state on allocation failure")
