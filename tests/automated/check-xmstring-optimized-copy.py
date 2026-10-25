#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringEntryCopy(_XmStringEntry entry)")
end = source.index("\n/** Begin macros", start)
body = source[start:end]
assert "size_t allocation_size" in body
assert "allocation_size > UINT_MAX" in body
assert "if (new_entry == NULL)" in body
assert "if (text == NULL)" in body
assert "XtFree((char *)new_entry);" in body
print("XmString optimized-entry copy guards present")
