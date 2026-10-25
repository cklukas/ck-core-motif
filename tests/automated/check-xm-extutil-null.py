#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c"
text = source.read_text()
assert "if (w == NULL)" in text
assert "if (w == NULL || preferred == NULL)" in text
assert "if (r_width != NULL)" in text
assert "if (r_height != NULL)" in text
assert "return(XtGeometryNo);" in text
print("Xm geometry helpers validate widget and output pointers")
