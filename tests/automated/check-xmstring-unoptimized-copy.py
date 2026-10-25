#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringEntryCopy(_XmStringEntry entry)")
end = source.index("\n/** Begin macros", start)
body = source[start:end]
assert "_XmUnoptSegRendBegins(new_entry) = NULL;" in body
assert "_XmUnoptSegRendEnds(new_entry) = NULL;" in body
assert body.count("SIZE_MAX / sizeof(XmStringTag)") >= 2
assert body.count("UINT_MAX / sizeof(XmStringTag)") >= 2
assert body.count("UINT_MAX / sizeof(_XmStringNREntry)") >= 1
assert "(Cardinal)(count * sizeof(XmStringTag))" in body
assert "(Cardinal)(segment_count * sizeof(_XmStringNREntry))" in body
assert body.count("_XmStringEntryFree(new_entry);") >= 2
assert "if (text == NULL)" in body
print("XmString unoptimized-entry copy guards present")
