#!/usr/bin/env python3
"""Check ColorSelector resource updates tolerate missing optional children."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()
set_start = source.index("SetValues(Widget current")
set_values = source[set_start:source.index("GeometryHandler", set_start)]
assert "if (XmColorS_sliders(csw)[i] != NULL)" in set_values
assert "if (XmColorS_chose_mode(csw)[i] != NULL)" in set_values
assert "XtVaSetValues(XmColorS_sliders(csw)[i]" in set_values
assert "XtVaSetValues(XmColorS_chose_mode(csw)[i]" in set_values

select_start = source.index("SelectColor(XmColorSelectorWidget csw)")
select = source[select_start:source.index("EndsInDigits", select_start)]
assert "if (XmColorS_list(csw) == NULL)" in select
assert select.index("if (XmColorS_list(csw) == NULL)") < select.index("XmListSelectPos")

print("ColorSelector optional child updates are null-safe")
