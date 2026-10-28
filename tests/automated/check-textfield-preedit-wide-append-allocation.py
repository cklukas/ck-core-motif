#!/usr/bin/env python3
"""Check wide XIM preedit append reallocations are bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("if (tf->text.overstrike && rest_len)")
start = source.index("if (tf->text.overstrike && rest_len)", start + 1)
end = source.index("\n        if (tf->text.overstrike && recover_len > 0)", start)
body = source[start:end]
recovery = source[end:source.index("\n  if (tf->text.max_char_size == 1)", end)]

for region in (body, recovery):
    assert "SIZE_MAX / sizeof(wchar_t)" in region
    assert "sizeof(wchar_t) >" in region
    assert "XtRealloc((char *)wc,\n" in region
assert "(insert_length+rest_len+1)*sizeof(wchar_t)" not in source

print("PASS TextField wide preedit append allocation bounds")
