#!/usr/bin/env python3
"""Check that XmString tag lengths are bounded before int conversion."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmStringIndexCacheTag(XmStringTag tag")
end = text.index("\n}\n", start)
region = text[start:end]

required = (
    "size_t tag_length;",
    "tag_length > (size_t)INT_MAX",
    "length = (int)tag_length",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("XmString tag length bounds missing: " + ", ".join(missing))

print("XmString tag length is bounded before int conversion")
