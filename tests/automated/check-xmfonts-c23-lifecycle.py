#!/usr/bin/env python3
"""Verify xmfonts C23 contracts and sample-font cleanup wiring."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "unsupported"
    / "xmfonts"
    / "xmfonts.c"
).read_text(encoding="utf-8")

assert "int main(int argc, char **argv)" in source
assert "Widget CreateApplication(Widget parent)" in source
assert "Widget CreateFontSample(Widget parent)" in source
assert "void SelectFontCB(Widget w, XtPointer client_data, XtPointer call_data)" in source
assert "void CloseCB(Widget w, XtPointer client_data, XtPointer call_data)" in source
assert "snprintf(message, sizeof(message), \"Unable to load font: %s\"" in source
assert "if (name) XtFree(name);" in source
assert "XmDIALOG_OK_BUTTON" in source
assert "XtAddCallback(button, XmNactivateCallback, CloseCB, (XtPointer)font);" in source
assert "if (font) XFreeFont (XtDisplay (w), font);" in source

print("xmfonts uses typed C23 callbacks and releases fonts when samples close")
