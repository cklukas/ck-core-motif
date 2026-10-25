#!/usr/bin/env python3
"""Check that implicit direction-pattern allocation is validated."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmString\nXmStringParseText(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "default_dir_pattern = XmParseMappingCreate(args, nargs);" in function
assert "if (default_dir_pattern == NULL)" in function
assert "XmStringFree(result);" in function
assert "return NULL;" in function

print("XmString parser validates implicit direction pattern allocation")
