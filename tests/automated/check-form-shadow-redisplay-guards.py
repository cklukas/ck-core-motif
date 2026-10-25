#!/usr/bin/env python3
"""Check XmForm shares safe prerequisites for resize and expose shadows."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Form.c").read_text()
start = source.index("static void \nResize(")
end = source.index("static void \nPlaceChildren(", start)
section = source[start:end]

assert "XtIsRealized((Widget)fw)" in section
assert "fw->manager.top_shadow_GC != NULL" in section
assert "fw->manager.bottom_shadow_GC != NULL" in section
assert "fw->manager.shadow_thickness != 0" in section
assert "fw->core.width != 0" in section
assert "fw->core.height != 0" in section
assert "if (draw_shadow && CanDrawShadow(fw))" in section
assert "if (!CanDrawShadow((XmFormWidget) fw))" in section
print("PASS Form shadow redisplay guards")
