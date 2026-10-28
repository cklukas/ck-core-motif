#!/usr/bin/env python3
"""Check debug XmString allocation handles failure and size overflow safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringI.h").read_text()
start = source.index("#ifdef _XmDEBUG_XMSTRING_MEM")
end = source.index("#else", start)
region = source[start:end]

assert "static char *_XmStrMallocDebug(size_t size)" in region
assert "size > (size_t)(Cardinal)~0 - STR_OFFSET" in region
assert "base = XtMalloc((Cardinal)(size + STR_OFFSET));" in region
assert "return base ? base + STR_OFFSET : NULL;" in region
assert "static void _XmStrFreeDebug(char *ptr)" in region
assert "if (ptr)" in region
assert "XtFree(ptr - STR_OFFSET);" in region
assert "XtMalloc((size) + STR_OFFSET) + STR_OFFSET" not in region
print("XmString debug allocation handles failure and overflow")
