#!/usr/bin/env python3
"""Check that XmString tag-cache entry allocation is handled safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(encoding="utf-8")
start = source.index("_XmStringIndexCacheTag(XmStringTag tag")
end = source.index("XmStringTag \n_XmStringCacheTag", start)
region = source[start:end]
assert "a = XtMalloc ((Cardinal)((size_t)length + 1));" in region
assert "if (a == NULL)" in region
assert "_XmProcessUnlock();" in region
assert "memcpy(a, tag, length);" in region
print("PASS XmString tag-cache allocation is guarded")
