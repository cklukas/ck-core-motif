#!/usr/bin/env python3
"""Check DataField value construction handles wide conversion failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
needle = "size_t converted = mbstowcs(XmTextF_wc_value(tf), start_temp"
start = source.index(needle)
body = source[start:source.index("XmTextF_string_length(tf) = (int)converted;", start)]

assert "converted == (size_t)-1" in body
assert "converted > (size_t)INT_MAX" in body
assert "XmTextF_wc_value(tf) = NULL" in body
assert "XmTextF_string_length(tf) = 0" in body
assert "XmTextF_size_allocd(tf) = 0" in body
wide_start = source.index("XmDataFieldSetStringWcs(\n        Widget")
wide_body = source[wide_start:wide_start + 2200]
assert "size_t result;" in wide_body
assert "result = wcstombs" in wide_body
setter = source[source.index("XmDataFieldSetString(\n        Widget"):]
assert "size_t measured_length;" in setter
assert "measured_length == (size_t)-1" in setter
assert "measured_length > (size_t)INT_MAX" in setter
assert "length = (int)measured_length" in setter

print("PASS DataField value conversion rejects invalid multibyte input")
