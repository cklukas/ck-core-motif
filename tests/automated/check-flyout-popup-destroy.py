#!/usr/bin/env python3
"""Check Flyout popup destruction clears stale shell and label pointers."""

from pathlib import Path


source = Path("lib/Xm/Flyout.c").read_text()

callback_start = source.index("static void popup_destroyed(")
callback = source[callback_start:source.index("static void Destroy(", callback_start)]
assert "flyout->flyout.popup_shell = NULL;" in callback
assert "flyout->flyout.label = NULL;" in callback
assert "flyout->flyout.visible = False;" in callback

popup_start = source.index("static void create_popup(")
popup = source[popup_start:source.index("static void Initialize(", popup_start)]
assert "XtNdestroyCallback" in popup
assert "popup_destroyed" in popup

print("Flyout popup destruction clears stale shell and label pointers")
