#!/usr/bin/env python3
"""Check individual XmString rendition entries before string access."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmeStringGetComponent(_XmStringContext context,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (begin_rends[tmp_index] == NULL)" in function
assert "if (end_rends[tmp_index] == NULL)" in function
assert function.count("_XmStrContError(context) = True;") >= 6

print("XmString rendition entries are validated")
