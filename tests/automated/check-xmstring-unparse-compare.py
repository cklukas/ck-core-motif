#!/usr/bin/env python3
"""Check that unparse comparison guards null component values."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("unparse_components(char          **result,", source.index("unparse_components(char          **result,") + 1)
end = source.index("\n}\n", start)
function = source[start:end]

assert "m_length != 0 && (m_value == NULL || p_value == NULL)" in function
assert function.index("m_value == NULL") < function.index("memcmp(m_value, p_value")

print("XmString unparse comparison guards null values")
