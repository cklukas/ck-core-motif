#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DrawUtils.c").read_text()
assert "#include <stdint.h>" in source
assert "unsigned int numAlloced" in source
assert "SIZE_MAX / sizeof(XRectangle)" in source
assert "XRectangle *new_alloced" in source
assert "if (new_alloced == NULL)" in source
assert "numAlloced = size;" in source
assert "XtRealloc" not in source
print("DrawUtils bevel-buffer growth guards present")
