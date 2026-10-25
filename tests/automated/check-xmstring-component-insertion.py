#!/usr/bin/env python3
"""Check that component creation validates its final segment insertion."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmStringComponentCreate(XmStringComponentType c_type,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (_XmStrEntryCount(str) == 0)" in function
assert "_XmStrFree(str);" in function
assert "_XmProcessUnlock();" in function
assert "return NULL;" in function

print("XmString component creation validates segment insertion")
