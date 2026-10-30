#!/usr/bin/env python3
"""Check rendition-tag extraction respects Xt's Cardinal allocation size."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/XmStringSeg.c").read_text(
    encoding="utf-8"
)

assert source.count("UINT_MAX / sizeof(XmStringTag)") >= 2

xmstring_source = (Path(__file__).parents[2] / "lib/Xm/XmString.c").read_text(
    encoding="utf-8"
)
for function, result in (
    ("_XmEntryRendEndGet", "return NULL;"),
    ("_XmEntryRendBeginGet", "return NULL;"),
    ("_XmEntryRendEndSet", "return;"),
    ("_XmEntryRendBeginSet", "return;"),
):
    start = xmstring_source.index("\n" + function + "(")
    end = xmstring_source.index("\n}\n", start)
    region = xmstring_source[start:end]
    assert "if (n < 0)" in region
    assert result in region

print("XmString rendition-tag accessors reject negative indices")
