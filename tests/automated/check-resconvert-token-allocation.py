#!/usr/bin/env python3
"""Check resource-converter token buffers handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text()
start = source.index("static char * \nGetNextToken(")
end = source.index("\n}", start)
body = source[start:end]
assert "buf = (char *) XtMalloc(1);" in body
assert "if (buf == NULL)" in body
assert "p = buf = XtMalloc(len + 1);" in body
assert body.count("if (buf == NULL)") >= 2
print("PASS resource-converter token allocations are guarded")
