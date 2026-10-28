#!/usr/bin/env python3
"""Check DataField selection replies validate state before display access."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_DoStuff(\n        Widget")
end = source.index("/* ARGSUSED */\nstatic void", start)
body = source[start:end]

assert "w == NULL" in body
assert "prim_select == NULL" in body
assert "type == NULL" in body
assert "length == NULL" in body
assert "format == NULL" in body
assert "*length != 0 && value == NULL" in body
assert "*format != 8" in body
assert "*length != 0 && *type == NULL_ATOM" in body
assert "size_t malloc_size;" in body
assert "piece_len > SIZE_MAX - malloc_size" in body
assert "piece_len > (SIZE_MAX - malloc_size) / sizeof(wchar_t)" in body
assert "malloc_size > UINT_MAX" in body
assert "converted == (size_t)-1" in body
assert "prim_select->num_chars += (int)converted" in body
assert "prim_select->num_chars = (int)converted" in body
assert "XFreeStringList(tmp_value);" in body
assert "malloc_size / sizeof(wchar_t)" in body
assert body.index("if (w == NULL") < body.index("XtDisplay(w)")
assert "prim_select->ref_count <= 0" in body
assert "--prim_select->ref_count" in body
assert "XtFree((char *)value);" in body

print("PASS DataField selection callback validates inputs")
