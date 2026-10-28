#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DataF.c").read_text()

assert "insert_length > SIZE_MAX - curr_length - 2" in source
assert "curr_length + insert_length + 2 > (size_t)UINT_MAX" in source
assert "newptr = XtMalloc(curr_length + insert_length + 2);" in source
