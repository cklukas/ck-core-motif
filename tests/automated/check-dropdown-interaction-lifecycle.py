#!/usr/bin/env python3
"""Check DropDown secondary interaction callbacks validate their inputs."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()

modify_start = source.index("ModifyVerifyTextField(Widget w")
modify = source[modify_start:source.index("/*\tFunction Name: ValueChangedTextField", modify_start)]
assert "cbw == NULL || field == NULL || field->text == NULL" in modify

value_start = source.index("ValueChangedTextField(Widget w")
value = source[value_start:source.index("/*\tFunction Name: ListSelected", value_start)]
assert "if (cbw == NULL)" in value

list_start = source.index("ListSelected(Widget w")
list_body = source[list_start:source.index("/*\tFunction Name: ShellButtonEvent", list_start)]
assert "XmDropDown_list(cbw) == NULL" in list_body

shell_start = source.index("ShellButtonEvent(Widget w")
shell = source[shell_start:source.index("/*\tFunction Name: LoseFocusHandler", shell_start)]
assert "XmDropDown_list(cbw) != NULL" in shell

focus_start = source.index("LoseFocusHandler(Widget w")
focus = source[focus_start:source.index("/*\tFunction Name: ComboUnpost", focus_start)]
assert "cbw == NULL || event == NULL" in focus

print("DropDown secondary interaction callbacks validate inputs")
