#!/usr/bin/env python3
"""Check XmCascadeButtonGadget bounds option-menu glyph geometry safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text()
start = source.index("static void \nDrawCascade(")
end = source.index("/*\n * redisplay the widget", start)
glyph = source[start:end]

assert "int width;" in glyph
assert "(int)CBG_Cascade_height(cb)" in glyph
assert "width -= 2 * (int)G_ShadowThickness(cb)" in glyph
assert "width <= 0" in glyph
assert "CBG_Cascade_height(cb) < height" in glyph
assert "XtIsRealized(XtParent(cb))" in glyph
assert "LabG_BackgroundGC(cb) == NULL" in glyph
assert "LabG_TopShadowGC(cb) == NULL" in glyph
assert "LabG_BottomShadowGC(cb) == NULL" in glyph
print("PASS CascadeButtonGadget cascade glyph guards")
