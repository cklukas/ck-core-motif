#!/usr/bin/env python3
"""Check XPM image scanning validates public entry arguments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text()
start = source.index("XpmCreateXpmImageFromImage(\n")
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]
guard = "display == NULL || xpmimage == NULL ||\n\t(image == NULL && shapeimage == NULL)"
assert guard in function
assert function.index(guard) < function.index("/* initialize pmap */")
assert "if (width == 0 || height == 0)" in function
assert function.index("if (width == 0 || height == 0)") < function.rindex("pmap.pixelindex =")
print("PASS XPM image-scan entry guard")
