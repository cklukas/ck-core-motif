#!/usr/bin/env python3
"""Check XIM registration failure paths do not leave partial state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()

set_start = source.index("static void \nset_current_xic")
set_end = source.index("\n}\n", set_start) + 2
set_region = source[set_start:set_end]
assert "if (add_ref(&xic_info->widget_refs, widget) == 0)" in set_region
assert set_region.index("if (add_ref(&xic_info->widget_refs, widget) == 0)") < set_region.index("XSaveContext")

register_start = source.index("XmImRegister(Widget w,")
register_end = source.index("\n}\n", register_start) + 2
register_region = source[register_start:register_end]
assert "registration = remember_widget(xim_info, w);" in register_region
assert "|| registration == NULL" in register_region
assert register_region.index("registration = remember_widget") < register_region.index("registration->external")

get_start = source.index("XmImGetXIC(Widget")
get_end = source.index("\n}\n", get_start) + 2
get_region = source[get_start:get_end]
assert "if (registration == NULL)" in get_region
assert "_XmAppUnlock(app);" in get_region

setxic_start = source.index("XmImSetXIC(Widget")
setxic_end = source.index("\n}\n", setxic_start) + 2
setxic_region = source[setxic_start:setxic_end]
assert "if (registration == NULL)" in setxic_region
assert setxic_region.index("if (registration == NULL)") < setxic_region.index("registration->external")

print("PASS XmIm registration lifecycle guards")
