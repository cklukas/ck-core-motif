#!/usr/bin/env python3
"""Check XmStringConcatAndFree separates aliased operands."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmStringConcatAndFree(XmString a,")
end = source.index("\n  _XmProcessLock();", start)
region = source[start:end]

assert "a != NULL && a == b" in region
assert "XmString duplicate = XmStringCopy(b);" in region
assert "XmStringFree(a);" in region
assert "b = duplicate;" in region

print("XmString self-concatenation separates aliased operands")
