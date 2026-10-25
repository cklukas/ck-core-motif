#!/usr/bin/env python3
"""Check that XmString unparse rejects non-empty null component values."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("unparse_text(char                **result,", source.index("unparse_text(char                **result,") + 1)
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (c_length != 0 && c_value == NULL)" in function
assert "return;" in function
assert function.index("if (c_length != 0 && c_value == NULL)") < function.index("memcpy")

print("XmString unparse rejects null component values")
