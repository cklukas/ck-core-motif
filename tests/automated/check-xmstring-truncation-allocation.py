#!/usr/bin/env python3
"""Check ASN.1 XmString truncation handles replacement allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("_XmStringTruncateASN1(")
region = source[start:]
assert "if (tmp == NULL)" in region
assert region.count("XtFree((char *)str);") >= 2
assert "unsigned char *tmp =" in region
assert region.index("if (tmp == NULL)") < region.index("memcpy(tmp")
print("PASS XmString ASN.1 truncation handles allocation failure")
