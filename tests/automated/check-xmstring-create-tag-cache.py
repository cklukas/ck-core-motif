#!/usr/bin/env python3
"""Check that XmString creation validates tag-cache results."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringNCreate(char *text,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmStringIndexGetTag(tag_index) == NULL" in function
assert "strcmp(_XmStringIndexGetTag(tag_index), curtag) != 0" in function
assert "if (_XmUnoptSegTag(&seg) == NULL)" in function
assert "_XmStrFree(str);" in function

print("XmString creation validates tag-cache results")
