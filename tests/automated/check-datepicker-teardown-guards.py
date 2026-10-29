#!/usr/bin/env python3
"""Check DatePicker popup and calendar teardown paths are lifecycle-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text()
changed = source[source.index("static void calendar_changed") : source.index("static void Initialize", source.index("static void calendar_changed"))]
destroy_start = source.index("static void Destroy(Widget widget)")
destroy = source[destroy_start : source.index("static Boolean SetValues", destroy_start)]
open_start = source.index("void XmDatePickerOpen")
open_body = source[open_start : source.index("void XmDatePickerClose", open_start)]
close_body = source[source.index("void XmDatePickerClose") :]
assert "popup_shell != NULL" in changed
assert "core.being_destroyed" in changed
assert "core.being_destroyed" in destroy
assert "picker->date_picker.calendar == NULL" in open_body
assert "core.being_destroyed" in close_body
print("PASS DatePicker popup and calendar teardown paths are guarded")
