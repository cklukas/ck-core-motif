#!/usr/bin/env python3
"""Check XPM serialization rejects a missing color table."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("static int\nCreateColors(")
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]

assert "ncolors != 0 && colors == NULL" in function
assert "return (XpmFileInvalid);" in function
print("PASS XPM color-table input guard")
