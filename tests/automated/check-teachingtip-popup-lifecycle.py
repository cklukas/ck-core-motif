#!/usr/bin/env python3
"""Check TeachingTip popup allocation and destruction lifecycle guards."""

from pathlib import Path


source = Path("lib/Xm/TeachingTip.c").read_text()

popup_start = source.index("static void create_popup(")
popup = source[popup_start:source.index("static void Initialize(", popup_start)]
assert "if (tip->teaching_tip.popup_shell == NULL)" in popup
assert "if (content == NULL)" in popup
assert "title_label == NULL" in popup
assert "text_label == NULL" in popup
assert "XtNdestroyCallback" in popup

callback_start = source.index("static void popup_destroyed(")
callback = source[callback_start:source.index("static void Initialize(", callback_start)]
assert "popup_shell = NULL" in callback
assert "title_label = NULL" in callback
assert "text_label = NULL" in callback
assert "visible = False" in callback

print("TeachingTip popup allocation and destruction are guarded")
