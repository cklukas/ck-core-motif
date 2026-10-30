#!/usr/bin/env python3
"""Check that tab-only compound-text prefixes do not append a newline."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("static unsigned char\nGetTextSegment(")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (separator)" in body
newline = body.index('strcat(newstring, "\\n")')
assert body.rfind("if (separator)", 0, newline) != -1
assert body.index("newstring = (char *)XtMalloc") < newline

print("compound-text tab prefix: newline is separator-only")
