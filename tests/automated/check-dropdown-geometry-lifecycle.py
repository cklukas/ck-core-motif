#!/usr/bin/env python3
"""Check DropDown geometry and action helpers handle missing children."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()

start = source.index("FindDesiredSize(Widget w")
end = source.index("/*\tFunction Name: CreateChildren", start)
geometry = source[start:end]
assert "else if (child != NULL)" in geometry
assert "label->width = label->height = label->border_width = 0;" in geometry
assert "text->width = text->height = text->border_width = 0;" in geometry
assert "arrow->width = arrow->height = arrow->border_width = 0;" in geometry

cancel_start = source.index("ComboCancel(Widget w")
cancel = source[cancel_start:source.index("/*\tFunction Name: FindComboBox", cancel_start)]
assert "XmDropDown_arrow(cbw) == NULL" in cancel
assert "XmDropDown_text(cbw) == NULL" in cancel

find_start = source.index("FindComboBox(Widget w)")
find = source[find_start:source.index("/*\tFunction Name: RegisterShellHandler", find_start)]
assert "if (w == NULL)" in find
register_start = source.index("RegisterShellHandler(Widget w)")
register = source[register_start:source.index("/*\tFunction Name:", register_start + 1)]
assert "cbw == NULL" in register

print("DropDown geometry and action helpers handle missing children")
