#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/uil/UilSarVal.c").read_text()
start = source.index("void\tsar_make_color_table")
end = source.index("void\tsar_make", start + 10) if "void\tsar_make" in source[start + 10:] else start + 6000
block = source[start:end]

assert "if (count == INT_MAX" in block
assert "UINT_MAX / sizeof(sym_color_element)" in block
assert "if (count < 0)" in block
