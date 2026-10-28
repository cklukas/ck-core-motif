#!/usr/bin/env python3
"""Check XPM data serialization validates dimensions before setup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("XpmCreateDataFromXpmImage(\n", source.index("#define RETURN"))
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]

guard = "data_return == NULL || image == NULL ||\n\timage->width == 0 || image->height == 0"
assert guard in function
assert function.index(guard) < function.index("*data_return = NULL;")
print("PASS XPM data dimension entry guard")
