#!/usr/bin/env python3
"""Check DropDown interaction callbacks validate widget and child state."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()

check = source[source.index("CheckUnpost(Widget w"):source.index("static void\nArrowClicked", source.index("CheckUnpost(Widget w"))]
assert "cbw == NULL || arrow == NULL || arrow->event == NULL" in check

arrow_start = source.index("ArrowClicked(Widget w")
arrow = source[arrow_start:source.index("#ifdef FIX_1446", arrow_start)]
assert "cbw == NULL || w == NULL" in arrow
assert "XmDropDown_popup_shell(cbw) == NULL" in arrow
assert "XmDropDown_text(cbw) == NULL" in arrow

text_start = source.index("IsTextOK(XmDropDownWidget cbw)")
text = source[text_start:source.index("/*\tFunction Name: VerifyTextField", text_start)]
assert "cbw == NULL || XmDropDown_text(cbw) == NULL" in text
assert "XmDropDown_list(cbw) == NULL" in text

verify_start = source.index("VerifyTextField(Widget w")
verify = source[verify_start:source.index("/*\tFunction Name: ModifyVerifyTextField", verify_start)]
assert "cbw == NULL || XmDropDown_text(cbw) == NULL" in verify
assert "XmDropDown_list(cbw) == NULL" in verify

print("DropDown interaction callbacks validate lifecycle state")
