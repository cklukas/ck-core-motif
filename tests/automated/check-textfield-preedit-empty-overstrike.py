#!/usr/bin/env python3
"""Check empty XIM preedit payloads have a wide buffer before overstrike."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nPreeditDraw(")
end = source.index("\n/*\n * This is the function set to XNPreeditCaretCallback", start)
body = source[start:end]

assert "} else {\n            wc = (wchar_t *)XtMalloc((Cardinal)sizeof(wchar_t));" in body
assert "wc[0] = (wchar_t)'\\0';" in body
assert body.index("wc = (wchar_t *)XtMalloc((Cardinal)sizeof(wchar_t));") < body.index("wcslen(wc)")
print("TextField empty preedit overstrike has an owned wide buffer")
