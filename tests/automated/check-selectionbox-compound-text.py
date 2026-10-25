#!/usr/bin/env python3
"""Check SelectionBox converts compound text before setting the child value."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SelectioB.c").read_text()
start = source.index("static void \nListCallback")
body = source[start:source.index("static void \nUpdateString", start)]
assert "textTrait == NULL || textTrait->setValue == NULL" in body
assert "XmCvtCTToXmString((char*) value)" in body
assert "textTrait -> setValue(SB_Text(sel), temp, XmFORMAT_XmSTRING);" in body
assert "textTrait -> setValue(SB_Text(sel), value, XmFORMAT_XmSTRING);" not in body
assert "update_success = False;" in body
assert "update_success = True;" in body
assert "XtFree((char*) value);" in body
print("PASS SelectionBox compound-text updates are guarded")
