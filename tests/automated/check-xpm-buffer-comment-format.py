#!/usr/bin/env python3
"""Check XPM buffer pixel comments use checked snprintf accounting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("    /* print pixels */")
end = source.index("    if (!WritePixels", start)
region = source[start:end]
assert "written = snprintf(ptr + used_size, ptr_size - used_size" in region
assert "written < 0 || (size_t)written >= ptr_size - used_size" in region
assert "used_size += (unsigned int)written;" in region
assert "VOID_SPRINTF" not in region
print("PASS XPM buffer pixel comments check formatting capacity")
