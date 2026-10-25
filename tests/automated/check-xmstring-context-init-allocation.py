#!/usr/bin/env python3
"""Check that XmString context initialization checks allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringInitContext(")
end = source.index("\n/*\n", start)
region = source[start:end]

assert "ct = (XmStringContext) XtMalloc" in region
assert "if (ct == NULL)" in region
assert region.index("if (ct == NULL)") < region.index("_XmStringContextReInit")
assert "return (FALSE);" in region
print("PASS XmString context initialization allocation is guarded")
