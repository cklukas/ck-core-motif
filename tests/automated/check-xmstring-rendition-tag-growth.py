#!/usr/bin/env python3
"""Check XmString rendition-tag growth rejects unrepresentable sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(
    encoding="utf-8"
)
for function in ("_XmEntryRendEndSet", "_XmEntryRendBeginSet"):
    start = source.index(function + "(")
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "old_count == INT_MAX" in region
    assert "SIZE_MAX / sizeof(XmStringTag)" in region
    assert "UINT_MAX / sizeof(XmStringTag)" in region
    assert "(Cardinal)((size_t)new_count * sizeof(XmStringTag))" in region
print("XmString rendition-tag growth checks count and allocation width")
