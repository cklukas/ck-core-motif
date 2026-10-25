#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringEntryCopy(_XmStringEntry entry)")
end = source.index("\n/** Begin macros", start)
body = source[start:end]
assert "size_t segment_count" in body
assert "SIZE_MAX / sizeof(_XmStringNREntry)" in body
assert "if (arr == NULL)" in body
assert "_XmStringEntryCopy" in body
assert "_XmStringEntryFree((_XmStringEntry)arr[j])" in body
print("XmString array-entry copy guards present")
