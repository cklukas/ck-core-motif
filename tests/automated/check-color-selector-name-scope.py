#!/usr/bin/env python3
"""Check that color-name replacement has one valid allocation variable."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c"
text = source.read_text(encoding="utf-8")
assert "char *new_name;" in text
assert "\t\tchar\t*new_name;" not in text
assert "new_name = XtNewString(XmColorS_colors(csw)[index].name);" in text
print("color selector replacement name has function scope")
