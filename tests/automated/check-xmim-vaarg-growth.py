#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmIm.c").read_text()
start = source.index("static void\nVaSetArg")
end = source.index("\n\nvoid\nXmImMbResetIC", start)
body = source[start:end]
assert "Cardinal new_max;" in body
assert "SIZE_MAX / sizeof(VaArg)" in body
assert "VaArg *new_args;" in body
assert "if (new_args == NULL)" in body
assert "list->max = new_max;" in body
print("XmIm VaArg-list growth guards present")
