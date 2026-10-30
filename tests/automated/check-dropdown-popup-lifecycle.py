#!/usr/bin/env python3
"""Check DropDown popup and list allocation lifecycle guards."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()

start = source.index("CreatePopup(Widget w, ArgList args")
body = source[start:source.index("/*\tFunction Name: PopdownList", start)]
assert "if (XmDropDown_popup_shell(cbw) == NULL)" in body
assert "if (XmDropDown_list(cbw) == NULL)" in body
assert "XmNdestroyCallback" in body
assert "PopupDestroyed" in body

callback_start = source.index("PopupDestroyed(Widget shell")
callback = source[callback_start:source.index("/*\tFunction Name: PopdownList", callback_start)]
assert "XmDropDown_popup_shell(cbw) = NULL;" in callback
assert "XmDropDown_list(cbw) = NULL;" in callback
assert "cbw->combo.hsb = NULL;" in callback
assert "cbw->combo.vsb = NULL;" in callback

print("DropDown popup and list allocation are guarded")
