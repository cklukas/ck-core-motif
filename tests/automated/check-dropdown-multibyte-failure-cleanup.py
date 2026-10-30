#!/usr/bin/env python3
"""Check DropDown frees every converted item when multibyte conversion fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()
start = source.index("len = mbstowcs")
end = source.index("conversion_failed:", start)
body = source[start:end]

assert "if (len == (size_t)(-1))" in body
assert "goto conversion_failed;" in body
cleanup = source[end:source.index("\n}", end)]
assert "XtFree((char *)parts[i]);" in cleanup

print("DropDown frees converted items after multibyte conversion failure")
