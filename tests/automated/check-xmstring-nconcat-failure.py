#!/usr/bin/env python3
"""Check that XmStringNConcat handles an allocation failure safely."""

from pathlib import Path


source = Path("lib/Xm/XmStringObso.c").read_text()
start = source.index("XmStringNConcat(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "tmp = XmStringConcat(first, second);" in function
assert "if (tmp == NULL)" in function
assert "return (NULL);" in function
assert "XmStringLength(first) + n" in function

print("XmStringNConcat propagates concat allocation failure")
