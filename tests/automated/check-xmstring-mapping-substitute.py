#!/usr/bin/env python3
"""Check transactional XmParseMapping substitute replacement."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmParseMappingSetValues(")
start = source.index("void \nXmParseMappingSetValues(", start)
end = source.index("\n}\n", start)
function = source[start:end]

assert "XmString requested" in function
assert "XmString replacement" in function
assert "if (requested != NULL && replacement == NULL)" in function
assert "XmStringFree(mapping->substitute);" in function

print("XmParseMapping substitute replacement is transactional")
