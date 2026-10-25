#!/usr/bin/env python3
"""Check that null parse patterns are treated as non-matches."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static Boolean\nmatch_pattern(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "else if (pattern->pattern == NULL)" in function
assert "return False;" in function
assert function.index("pattern->pattern == NULL") < function.index("strlen((char*) pattern->pattern)")

print("XmString pattern matching guards null patterns")
