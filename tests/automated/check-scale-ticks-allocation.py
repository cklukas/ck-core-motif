#!/usr/bin/env python3
"""Check XmScaleSetTicks handles local tick-array allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScalTics.c").read_text(
    encoding="utf-8"
)
start = source.index("sep = (Widget*) ALLOCATE_LOCAL")
end = source.index("if (orient == XmHORIZONTAL)", start)
region = source[start:end]
assert "if (sep == NULL)" in region
assert "_XmAppUnlock(app);" in region
assert "return;" in region
print("scale tick generation handles local array allocation failure")
