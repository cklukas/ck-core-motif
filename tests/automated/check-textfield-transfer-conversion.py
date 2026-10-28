#!/usr/bin/env python3
"""Check TextField transfer conversion results and termination."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("DoStuff(Widget w, ")
end = source.index("static void \nHandleTargets", start)
body = source[start:end]

assert "size_t converted;" in body
assert body.count("converted = mbstowcs") >= 3
assert body.count("converted == (size_t)-1") >= 3
assert "(int)mbstowcs" not in body
assert "prim_select->num_chars < 0" not in body
assert "wc_value[converted] = (wchar_t)0L" in body

print("PASS TextField transfer conversion guards")
