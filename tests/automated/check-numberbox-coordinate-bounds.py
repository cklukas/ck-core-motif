#!/usr/bin/env python3
"""NumberBox bounds native child allocation to representable coordinates."""
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/NumberBox.c").read_text()
assert "MIN(number->core.width,SHRT_MAX)" in source
assert "MIN(number->core.height,SHRT_MAX)" in source
assert "arrow_width>0 && field_height>=2" in source
assert "Dimension editor_width_dimension" in source
assert "(Position)MAX(0,MIN(height-1,field_height+gap))" in source
assert "(XmTextPosition)strlen(text)" in source
assert "XFillPolygon" not in source
print("PASS NumberBox bounds native child allocation")
