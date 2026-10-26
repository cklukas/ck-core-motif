#!/usr/bin/env python3
"""Check XmTextFindStringWcs terminates converted search text."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFind.c").read_text()
start = source.index("Boolean\nXmTextFindStringWcs(")
end = source.index("\n\n", source.index("return(return_val);", start))
body = source[start:end]

assert "converted < allocation_size" in body
assert "string[converted] = '\\0';" in body
assert "return_val = XmTextFindString" in body

print("XmTextFindStringWcs terminates converted search text")
