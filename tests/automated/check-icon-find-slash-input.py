#!/usr/bin/env python3
"""Check IconFile path scanning rejects a null string before charset logic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("find_slash(String str)")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (str == NULL)" in function
assert function.index("str == NULL") < function.index("MB_CUR_MAX")
print("PASS IconFile find_slash null input guard")
