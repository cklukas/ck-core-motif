#!/usr/bin/env python3
"""Check that ASN.1 truncation rejects used-length overflow."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringTruncateASN1(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "delta = length + header;" in function
assert "delta > (unsigned short)~0 - used" in function
assert "XtFree((char *)str);" in function
assert "return (NULL);" in function

print("ASN.1 truncation rejects used-length overflow")
