#!/usr/bin/env python3
"""Check that empty text components do not pass null to memcpy."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringGetTextConcat(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (len != 0)" in function
assert "memcpy( &OutStr[OldLen]" in function
assert function.index("if (len != 0)") < function.index("memcpy( &OutStr[OldLen]")

print("XmString text concatenation skips empty null values")
