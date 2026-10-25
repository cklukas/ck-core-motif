#!/usr/bin/env python3
"""Check XmParseMappingCreate handles constructor allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmParseMapping\nXmParseMappingCreate")
end = source.index("void \nXmParseMappingSetValues", start)
region = source[start:end]
assert "XmParseMapping result = XtNew(_XmParseMappingRec);" in region
assert "if (result == NULL)" in region
assert region.index("if (result == NULL)") < region.index("bzero")
print("PASS XmString parse-mapping construction handles allocation failure")
