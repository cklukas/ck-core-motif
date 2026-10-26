#!/usr/bin/env python3
"""Check wide modify-verify callbacks are copied before conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("if (wcs_tvcb.text->length) {")
end = source.index("\n      } else {\n\tnewblock->ptr = NULL;", start)
body = source[start:end]

assert "wchar_t *wide_copy = NULL;" in body
assert "wcs_tvcb.text->wcsptr == NULL" in body
assert "wide_copy = (wchar_t *)XtMalloc" in body
assert "wide_copy[wide_length] = (wchar_t)0L;" in body
assert "wcstombs(newblock->ptr,\n\t\t\t\twide_copy, capacity)" in body
assert "XtFree((char *)wide_copy);" in body

print("TextStrSo wide callback text is copied safely")
