#!/usr/bin/env python3
"""Check DataField invalid wchar warnings cannot index scratch out of bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_ValidateString(")
end = source.index("\n/* The following is a hack", start)
body = source[start:end]
assert body.count("(size_t)csize < sizeof(scratch)") == 2
assert "scratch[0] = '\\0';" in body
assert "scratch[csize]= '\\0';" not in body

print("DataField invalid wchar warnings stay within scratch")
