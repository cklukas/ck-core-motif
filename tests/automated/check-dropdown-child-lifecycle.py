#!/usr/bin/env python3
"""Check DropDown lifecycle updates tolerate missing internal children."""

from pathlib import Path


source = Path("lib/Xm/DropDown.c").read_text()
initialize = source[source.index("Initialize(Widget req"):source.index("static void \nResize", source.index("Initialize(Widget req"))]
assert "XmDropDown_text(cbw) != NULL" in initialize
assert "XtAddEventHandler(XmDropDown_text(cbw)" in initialize

get_values = source[source.index("GetValuesHook(Widget w"):source.index("/*\tFunction Name: SetValues", source.index("GetValuesHook(Widget w"))]
assert "XmDropDown_list(cbw) != NULL" in get_values

set_values_start = source.index("SetValues(Widget current")
set_values = source[set_values_start:source.index("#ifdef FIX_1170", set_values_start)]
for guard in (
    "XmDropDown_list(set_cbw) != NULL",
    "XmDropDown_text(set_cbw) != NULL",
    "XmDropDown_arrow(set_cbw) != NULL",
    "XmDropDown_label(old_cbw) != NULL",
    "XmDropDown_label(set_cbw) != NULL",
):
    assert guard in set_values, guard
assert "Dimension width = 0, height = 0;" in set_values

print("DropDown child lifecycle updates are null-safe")
