#!/usr/bin/env python3
"""Check DatePicker keeps its popup Calendar direction synchronized."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text()
create_start = source.index("static void create_popup(")
create_end = source.index("static void position_popup(", create_start)
create = source[create_start:create_end]
set_start = source.index("static Boolean SetValues(")
set_end = source.index("static void Redisplay(", set_start)
set_values = source[set_start:set_end]

assert "XmNcalendarRtl" in create
assert "XmPrim_layout_direction(picker)" in create
assert "Boolean direction_changed = old->primitive.layout_direction !=" in set_values
assert "XtVaSetValues(picker->date_picker.calendar," in set_values
assert "||\n           direction_changed;" in set_values
print("PASS DatePicker synchronizes popup Calendar direction")
