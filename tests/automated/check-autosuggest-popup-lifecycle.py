#!/usr/bin/env python3
"""Check AutoSuggestBox popup creation and destruction lifecycle guards."""

from pathlib import Path


source = Path("lib/Xm/AutoSuggestBox.c").read_text()

popup_start = source.index("static void create_popup(")
popup = source[popup_start:source.index("static void emit_query(", popup_start)]
assert "if (box->auto_suggest.popup_shell == NULL)" in popup
assert "XtNdestroyCallback" in popup
assert "popup_destroyed" in popup

callback_start = source.index("static void popup_destroyed(")
callback = source[callback_start:source.index("static void emit_query(", callback_start)]
assert "box->auto_suggest.popup_shell = NULL;" in callback
assert "box->auto_suggest.list = NULL;" in callback

print("AutoSuggestBox popup creation and destruction are guarded")
