#!/usr/bin/env python3
"""Check IconGadget visual resource updates preserve old values on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()
start = source.index("static\tBoolean\nSetValues(")
end = source.index("\n    if (IG_LargeIconMask(nw)", start)
region = source[start:end]

assert "XmRenderTable render_table;" in region
assert "render_table = XmRenderTableCopy" in region
assert "if (render_table != NULL)" in region
assert "IG_RenderTable(nw) = IG_RenderTable(cw);" in region
assert "XmString label_string;" in region
assert "label_string = IG_LabelString(nw) != NULL" in region
assert "if (label_string != NULL)" in region
assert "IG_LabelString(nw) = IG_LabelString(cw);" in region
assert "IG_RenderTable(nw) != NULL && IG_LabelString(nw) != NULL" in region
print("IconGadget visual resources preserve old values on copy failure")
