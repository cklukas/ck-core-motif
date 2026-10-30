#!/usr/bin/env python3
"""Check ColorSelector mode lifecycle handles missing toggle children."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()

initialize_start = source.index("Initialize(Widget request")
initialize = source[initialize_start:source.index("SetValues(Widget current", initialize_start)]
assert "button = XmColorS_chose_mode(csw)[XmColorS_color_mode(csw)];" in initialize
assert "if (button != NULL)" in initialize
assert initialize.index("if (button != NULL)") < initialize.index("XmToggleButtonSetState(button")

set_values_start = source.index("SetValues(Widget current")
set_values = source[set_values_start:source.index("GeometryHandler", set_values_start)]
assert "if (XmColorS_chose_mode(csw)[XmColorS_color_mode(csw)] != NULL)" in set_values
assert "XmToggleButtonSetState(" in set_values

callback_start = source.index("change_mode(Widget w")
callback = source[callback_start:source.index("new_mode(XmColorSelectorWidget csw, XmColorMode mode)", callback_start)]
assert "w == NULL || csw_ptr == NULL || tp == NULL" in callback
assert callback.index("return;") < callback.index("toggle->reason")

print("ColorSelector mode lifecycle is null-safe")
