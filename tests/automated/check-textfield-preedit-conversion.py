#!/usr/bin/env python3
"""Check TextField preedit callback conversions are validated and terminated."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nPreeditDraw(")
end = source.index("/*\n * This is the function set to XNPreeditCaretCallback", start)
body = source[start:end]

assert "converted = mbstowcs(wc" in body
assert "converted == (size_t)-1" in body
assert "wc[converted] = (wchar_t) '\\0'" in body
assert "converted = wcstombs(recover_mb" in body
assert "recover_mb[converted] = '\\0'" in body
assert "converted = mbstowcs(recover_wc" in body
assert "wcstombs(recover_mb" in body

print("PASS TextField preedit conversion guards")
