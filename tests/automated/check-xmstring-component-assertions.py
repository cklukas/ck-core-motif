#!/usr/bin/env python3
"""Check malformed XmString components do not assert-abort iteration."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmeStringGetComponent(_XmStringContext context,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "assert(False);" not in function
assert function.count("_XmStrContError(context) = True;") >= 10
assert "return XmSTRING_COMPONENT_END;" in function

print("XmString component iteration handles malformed state without asserts")
