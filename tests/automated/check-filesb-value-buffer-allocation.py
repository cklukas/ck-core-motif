#!/usr/bin/env python3
"""Check FileSB full-path value construction has a safe fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "valueString = XtMalloc((Cardinal)(dirLen + fileLen + 1)) ;"
start = source.index(allocation)
guard = source.index("if(    valueString != NULL    )", start)
fallback = source.index("valueString = fileStr ;", guard)
assert start < guard < fallback
dir_decl = source.rfind("String dirStr", 0, start)
assert dir_decl >= 0
assert "if(    dirStr == NULL    )" in source[dir_decl:guard]
assert "dirLen > SIZE_MAX - fileLen - 1" in source[dir_decl:guard]
assert "dirLen + fileLen + 1 > UINT_MAX" in source[dir_decl:guard]
assert "XtFree( dirStr) ;" in source[guard:source.index("} ", guard) + 2]
print("PASS FileSB full-path value allocation has a fallback")
