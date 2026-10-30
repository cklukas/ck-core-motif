#!/usr/bin/env python3
"""Check DropDown popup and text/list conversion helpers validate children."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()

popup_start = source.index("PopupList(Widget w)")
popup = source[popup_start:source.index("/*\tFunction Name: SetListFromText", popup_start)]
assert "XmDropDown_text(cbw) == NULL" in popup
assert "XmDropDown_arrow(cbw) == NULL" in popup

list_start = source.index("SetListFromText(Widget w")
list_body = source[list_start:source.index("/*\tFunction Name: SetTextFromList", list_start)]
assert "XmDropDown_text(cbw) == NULL" in list_body
assert "XmDropDown_list(cbw) == NULL" in list_body

text_start = source.index("SetTextFromList(Widget w)")
text_body = source[text_start:source.index("/************************************************************", text_start)]
assert "XmDropDown_text(cbw) == NULL" in text_body
assert "XmDropDown_list(cbw) == NULL" in text_body

value_start = source.index("XmDropDownGetValue(Widget w)")
value = source[value_start:source.index("/*\tFunction Name:", value_start + 1)]
assert "XmDropDown_text(cbw) == NULL" in value
assert "return NULL;" in value

print("DropDown conversion helpers validate child lifecycle state")
