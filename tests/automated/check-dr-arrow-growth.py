#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DrArrow.c").read_text()
assert "XRectangle *new_top, *new_cent, *new_bot;" in source
assert "if (new_top == NULL || new_cent == NULL || new_bot == NULL)" in source
assert source.count("XtFree((char *)new_") >= 3
assert "memcpy(new_top, top" in source
assert "allocated = size;" in source
assert "XtRealloc" not in source
print("DrArrow geometry-array growth guards present")
