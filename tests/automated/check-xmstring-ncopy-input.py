#!/usr/bin/env python3
"""Check that XmStringNCopy rejects invalid inputs before conversion."""

from pathlib import Path


source = Path("lib/Xm/XmStringObso.c").read_text()
start = source.index("XmStringNCopy(\n        XmString str,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (str == NULL || n < 0)" in function
assert "return (NULL);" in function
assert function.index("if (str == NULL || n < 0)") < function.index("XmCvtXmStringToByteStream")

print("XmStringNCopy rejects null and negative-length inputs")
