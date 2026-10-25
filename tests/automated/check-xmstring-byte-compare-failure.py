#!/usr/bin/env python3
"""Check that XmStringByteCompare handles conversion allocation failure."""

from pathlib import Path


source = Path("lib/Xm/XmStringObso.c").read_text()
start = source.index("XmStringByteCompare(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "unsigned char  *a = NULL;" in function
assert "unsigned char  *b = NULL;" in function
assert "a_length == 0 && a == NULL" in function
assert "b_length == 0 && b == NULL" in function
assert "return (FALSE);" in function

print("XmStringByteCompare rejects conversion failures safely")
