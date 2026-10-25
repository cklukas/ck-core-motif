#!/usr/bin/env python3
"""Check that copied XmString segments size wide text by wchar_t width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("/* Copy the text. */")
end = source.index("*text = (XtPointer) tmp;", start)
region = source[start:end]
assert "type == XmWIDECHAR_TEXT" in region
assert "sizeof(wchar_t)" in region
assert "memcpy(tmp, *text, text_bytes);" in region
assert "memcpy(tmp, *text, *char_count);" not in region
print("copied wide XmString segments use wchar_t-sized storage")
