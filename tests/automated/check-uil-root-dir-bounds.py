#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/uil/UilDB.c").read_text()
start = source.index("String get_root_dir_name")
end = source.index("/*\n * XAPPLRES_DEFAULT", start)
block = source[start:end]

assert "size_t length;" in block
assert "length > (size_t)UINT_MAX - 2" in block
assert "XtMalloc ((Cardinal)(length + 2))" in block
assert "if (outptr == NULL)" in block
