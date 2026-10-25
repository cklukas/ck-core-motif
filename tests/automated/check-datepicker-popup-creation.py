#!/usr/bin/env python3
"""Check DatePicker lazy popup creation failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DatePicker.c"
text = source.read_text(encoding="utf-8")

start = text.index("static void create_popup")
body = text[start:text.index("static void position_popup", start)]
assert "if (!picker->date_picker.popup_shell)" in body
assert "if (!picker->date_picker.calendar)" in body
assert "XtDestroyWidget(picker->date_picker.popup_shell);" in body
assert "picker->date_picker.popup_shell = NULL;" in body
print("DatePicker popup creation failures are cleaned up")
