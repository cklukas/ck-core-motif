#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/FontS.c").read_text()
assert source.count("SIZE_MAX / sizeof(*new_list)") >= 2
assert "new_list = (FontData *) XtRealloc" in source
assert "new_list = (String *) XtRealloc" in source
print("FontS list-size bounds present")
