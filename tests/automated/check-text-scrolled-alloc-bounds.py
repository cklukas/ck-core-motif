#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Text.c").read_text()

assert "size_t name_size;" in source
assert "name_size > (size_t)UINT_MAX - 3" in source
assert "(size_t)arg_size > (size_t)UINT_MAX / sizeof(Arg)" in source
assert "s_size = (Cardinal)(name_size + 3);" in source
