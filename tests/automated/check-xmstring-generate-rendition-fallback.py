#!/usr/bin/env python3
"""Check transactional fallback rendition decoration."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmString\nXmStringGenerate(")
end = source.index("\n}\n", start)
function = source[start:end]

assert function.count("XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN") == 1
assert function.count("XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END") == 1
assert function.count("if (result == NULL)") >= 3
assert "XmStringFree(tmp_1);" in function

print("XmStringGenerate fallback rendition decoration is transactional")
