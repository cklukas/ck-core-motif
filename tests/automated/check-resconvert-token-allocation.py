#!/usr/bin/env python3
"""Check resource token buffers handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text()
start = source.rindex("GetNextToken(")
body = source[start:source.index("/*ARGSUSED*/", start)]
assert "buf = (char *) XtMalloc(1);" in body
assert "if (buf == NULL)" in body
assert "p = buf = XtMalloc(len + 1);" in body
assert body.count("if (buf == NULL)") == 2
assert "*buf = '\\0';" in body
print("PASS resource token allocations are guarded")
