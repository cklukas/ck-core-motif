#!/usr/bin/env python3
"""Check that copied XmString segments take the byte count of the component for every text type."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("Copy the text.")
end = source.index("\n  /* Free the local context. */", start)
region = source[start:end]
assert "text_bytes = (size_t)*char_count;" in region
assert "memcpy(copied_text, *text, text_bytes);" in region
# the count is already a number of bytes: it must not be multiplied for wide text
assert "(size_t)*char_count * sizeof(wchar_t)" not in region
assert "XmWIDECHAR_TEXT" not in region
# the copy ends with a null wide character after a whole number of wide characters
assert "2 * sizeof(wchar_t) - 1" in region
assert "bzero(copied_text + text_bytes, padded_bytes - text_bytes);" in region
print("copied wide XmString segments are copied by their byte count and padded")
