#!/usr/bin/env python3
"""Check invalid XmString entry types do not assert or dereference null."""

from pathlib import Path


string_source = Path("lib/Xm/XmString.c").read_text()
start = string_source.index("_XmEntryByteCountGet(_XmStringEntry entry)")
end = string_source.index("\n}\n", start)
function = string_source[start:end]
assert "if (entry == NULL)" in function
assert "return 0;" in function
assert "assert(FALSE);" not in function

segment_source = Path("lib/Xm/XmStringSeg.c").read_text()
start = segment_source.index("else if (ctype == XmSTRING_COMPONENT_WIDECHAR_TEXT)")
end = segment_source.index("/* Force a tag", start)
region = segment_source[start:end]
assert "result = FALSE;" in region
assert "done = TRUE;" in region
assert "assert(FALSE);" not in region

print("XmString invalid entry types fall back safely")
