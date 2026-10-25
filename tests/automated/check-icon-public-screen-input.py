#!/usr/bin/env python3
"""Check XmGetIconFileName validates screen before display access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("XmGetIconFileName(")
start = source.index("String", start - 80)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (screen == NULL)" in function
assert "return NULL;" in function
assert function.index("screen == NULL") < function.index("DisplayOfScreen")
print("PASS XmGetIconFileName screen input guard")
