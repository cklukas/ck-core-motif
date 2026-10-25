#!/usr/bin/env python3
"""Check that parser substitute-copy failure aborts cleanly."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static Boolean\nparse_pattern(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "insertion = XmStringCopy(pat->substitute);" in function
assert "if (pat->substitute != NULL && insertion == NULL)" in function
assert "XmStringFree(*result);" in function
assert "*result = NULL;" in function

print("XmString parser propagates substitute-copy failure")
