#!/usr/bin/env python3
"""Check optimized XmString paths avoid invalid layout casts and scope bugs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c"
text = source.read_text(encoding="utf-8")
assert "seg.header.rend_begin = _XmStrRendBegin((_XmString)string);" in text
assert "seg.header.rend_end = _XmStrRendEnd((_XmString)string);" in text
assert "seg.header.byte_count = _XmStrByteCount((_XmString)string);" in text
assert "if (allocation_size > UINT_MAX) {" in text
assert "if (entry_len > sizeof(XtPointer)) {" in text
assert "((a_type != XmNO_TEXT) && b_tabs == 0)" in text
assert "if (tag_count > hits) {" in text
assert "XtFree((XtPointer)tags);" in text
print("XmString optimized and rendition-stack warning paths are guarded")
