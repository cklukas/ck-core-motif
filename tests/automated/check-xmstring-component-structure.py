#!/usr/bin/env python3
"""Check malformed XmString component structure guards."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmeStringGetComponent(_XmStringContext context,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (str == NULL)" in function
assert function.count("if (_XmStrEntry(str) == NULL)") >= 2
assert "if (line == NULL)" in function
assert "if (seg == NULL)" in function
assert "begin_count != 0 && begin_rends == NULL" in function
assert "end_count != 0 && end_rends == NULL" in function

print("XmString component structure guards are present")
