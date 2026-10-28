#!/usr/bin/env python3
"""Check XPM serialization rejects a missing color table."""
from pathlib import Path

root = Path(__file__).parents[2]
source = (root / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("static int\nCreateColors(")
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]

assert "ncolors != 0 && colors == NULL" in function
assert "return (XpmFileInvalid);" in function
buffer_source = (root / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
write_start = buffer_source.index("static int\nWriteColors(")
write_start = buffer_source.index("{", write_start)
write_end = buffer_source.index("\n}", write_start)
write_function = buffer_source[write_start:write_end]
assert "ncolors != 0 && colors == NULL" in write_function
assert "return (XpmFileInvalid);" in write_function
print("PASS XPM color-table input guard")
