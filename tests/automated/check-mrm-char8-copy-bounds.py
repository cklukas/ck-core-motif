#!/usr/bin/env python3
"""Check Mrm Char8 copies bound strlen before Xt allocation narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text()
start = source.index("Urm__CW_SafeCopyValue (long")
body = source[start:source.index("\n}", start)]

assert "char8_length;" in body
assert "char8_length = strlen(char8_src);" in body
assert "char8_length >= (size_t)UINT_MAX" in body
assert "XtMalloc ((Cardinal)(char8_length + 1))" in body
assert body.index("char8_length >=") < body.index("XtMalloc ((Cardinal)(char8_length + 1))")
print("Mrm Char8 copy allocation is width-bounded")
