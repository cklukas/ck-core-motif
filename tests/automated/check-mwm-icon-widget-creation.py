#!/usr/bin/env python3
"""Check MWM icon-box widget creation before child use."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

needle = 'iconWidget =  XtCreateManagedWidget("iconInIconBox"'
start = text.index(needle)
body = text[start:text.index("    if (dummyString == NULL)", start)]
assert "if (iconWidget == NULL)" in body
assert "MWarning" in body
assert "return (NULL);" in body
print("MWM icon-box widget creation is guarded")
