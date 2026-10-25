#!/usr/bin/env python3
"""Check ColorObj selection callbacks before atom/value access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nGetSelection")
end = source.index("/**********************************************************************/\n/** UpdatePixelSet", start)
region = source[start:end]
assert "w == NULL" in region
assert "selection == NULL" in region
assert "atoms == NULL" in region
assert "numScreens <= 0" in region
assert "value != NULL && length != NULL" in region
print("PASS ColorObj selection callback inputs are guarded")
