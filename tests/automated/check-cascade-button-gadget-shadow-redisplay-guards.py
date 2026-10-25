#!/usr/bin/env python3
"""Check XmCascadeButtonGadget validates its shadow draw state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text()
start = source.index("static void \nDrawShadow(")
end = source.index("static void \nDrawCascade(", start)
shadow = source[start:end]

assert "XtIsRealized(XtParent(cb))" in shadow
assert "LabG_TopShadowGC(cb) != NULL" in shadow
assert "LabG_BottomShadowGC(cb) != NULL" in shadow
assert "cb->gadget.shadow_thickness != 0" in shadow
assert "cb->rectangle.width > 2 * cb->gadget.highlight_thickness" in shadow
assert "cb->rectangle.height > 2 * cb->gadget.highlight_thickness" in shadow
print("PASS CascadeButtonGadget shadow redisplay guards")
