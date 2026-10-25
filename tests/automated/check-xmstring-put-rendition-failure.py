#!/usr/bin/env python3
"""Check XmStringPutRendition reports component-allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringPutRendition(")
end = source.index("\nvoid \nXmParseMappingGetValues", start)
region = source[start:end]

assert region.count("if (tmp1 == NULL)") == 2
assert "if (tmp2 == NULL)" in region
assert "XmStringFree(tmp2);" in region
print("PASS XmStringPutRendition fails cleanly on component allocation")
