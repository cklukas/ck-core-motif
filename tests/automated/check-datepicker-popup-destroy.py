#!/usr/bin/env python3
"""Check DatePicker popup destruction clears stale child pointers."""

from pathlib import Path


source = Path("lib/Xm/DatePicker.c").read_text()

callback_start = source.index("static void popup_destroyed(")
callback = source[callback_start:source.index("static void create_popup(", callback_start)]
assert "picker->date_picker.popup_shell = NULL;" in callback
assert "picker->date_picker.calendar = NULL;" in callback
assert "picker->date_picker.open = False;" in callback

popup_start = source.index("static void create_popup(")
popup = source[popup_start:source.index("static void position_popup(", popup_start)]
assert "XtNdestroyCallback" in popup
assert "popup_destroyed" in popup

print("DatePicker popup destruction clears stale shell and calendar pointers")
