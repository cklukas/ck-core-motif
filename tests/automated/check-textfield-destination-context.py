#!/usr/bin/env python3
"""Check TextField destination-context allocations before use."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static TextFDestData \nGetTextFDestData")
end = source.index("\nvoid\n_XmTextFToggleCursorGC", start)
body = source[start:end]

assert "ctx_data = (XmTextContextData) XtMalloc" in body
assert "if (ctx_data == NULL)" in body
assert "dest_data = (TextFDestData) XtCalloc" in body
assert "if (dest_data == NULL)" in body
assert "XtFree((char *)ctx_data);" in body

print("TextField destination-context allocations are checked")
