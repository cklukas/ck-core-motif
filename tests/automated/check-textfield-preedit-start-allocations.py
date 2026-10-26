#!/usr/bin/env python3
"""Check TextField preedit-start overstrike allocation guards."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static int\nPreeditStart(")
end = source.index("\n\n/*\n * This is the function set to XNPreeditDoneCallback", start)
body = source[start:end]

assert "over_len < 0" in body
assert "SIZE_MAX / sizeof(wchar_t)" in body
assert body.count("if (mb == NULL)") == 1
assert body.count("if (wc == NULL)") == 1
assert body.count("under_preedit = False;") >= 3

print("TextField preedit-start allocations are checked")
