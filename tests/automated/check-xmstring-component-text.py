#!/usr/bin/env python3
"""Check non-empty XmString text pointers before component access."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmeStringGetComponent(_XmStringContext context,")
end = source.index("\n}\n", start)
function = source[start:end]

assert function.count("char_count != 0 && seg_text == NULL") >= 2
assert function.count("_XmStrContError(context) = True;") >= 8

print("XmString component text pointers are validated")
