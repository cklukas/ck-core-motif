#!/usr/bin/env python3
"""Check XPM data extension writes propagate formatting/input failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text(
    encoding="utf-8"
)
assert "LFUNC(CreateExtensions, int" in source
call = source[source.index("if (extensions && !CreateExtensions") : source.index(
    "*data_return = data", source.index("if (extensions && !CreateExtensions")
)]
assert "RETURN(XpmFileInvalid);" in call
start = source.index("static int\nCreateExtensions(")
body = source[start:]
assert "if (dataptr == NULL || *dataptr == NULL" in body
assert "written = snprintf(*dataptr, (size_t)data_size - used" in body
assert "written < 0 || (size_t)written >= (size_t)data_size - used" in body
assert "return (0);" in body
assert "return (1);" in body
print("PASS XPM data extension writes propagate status")
