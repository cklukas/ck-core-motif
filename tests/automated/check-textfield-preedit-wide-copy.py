#!/usr/bin/env python3
"""Check wide XIM preedit payloads are copied by advertised length."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("if (!call_data->text->encoding_is_wchar)")
end = source.index("if (tf->text.overstrike && rest_len)", start)
body = source[start:end]

assert "memcpy(wc, call_data->text->string.wide_char," in body
assert "(size_t)insert_length * sizeof(wchar_t)" in body
assert "wcscpy(wc, call_data->text->string.wide_char)" not in body
assert "wc[insert_length] = (wchar_t) '\\0';" in body

print("PASS TextField wide preedit copy bounds")
