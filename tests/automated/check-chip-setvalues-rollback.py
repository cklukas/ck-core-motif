#!/usr/bin/env python3
"""Check Chip SetValues preserves resources when replacement copies fail."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Chip.c").read_text()
start = source.rindex("static Boolean SetValues")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]

font_copy = body.index("XmFontList new_font")
assert "if (new_font)" in body[font_copy:]
assert "chip->chip.font = old->chip.font;" in body[font_copy:]
assert body.index("if (old->chip.font) XmRenderTableFree", font_copy) > body.index(
    "if (new_font)", font_copy
)

label_copy = body.index("XmString replacement")
assert "if (replacement)" in body[label_copy:]
assert "chip->chip.label = old->chip.label;" in body[label_copy:]
assert body.index("XmStringFree(old->chip.label)", label_copy) > body.index(
    "if (replacement)", label_copy
)

print("Chip SetValues replacement failures preserve live resources")
