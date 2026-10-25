#!/usr/bin/env python3
"""Check XmStringPutRendition propagates source-copy failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringPutRendition(")
end = source.index("\nvoid \nXmParseMappingGetValues", start)
region = source[start:end]

assert "copied;" in region
assert "copied = XmStringCopy(string);" in region
assert "if (copied == NULL)" in region
assert region.index("if (copied == NULL)") < region.index("XmStringConcatAndFree(tmp1, copied)")
print("PASS XmStringPutRendition propagates source-copy failure")
