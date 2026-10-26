#!/usr/bin/env python3
"""Check XmText preedit-start overstrike snapshot handling."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("static int\nPreeditStart(")
end = source.index("\n\n/*\n * This is the function set to XNPreeditDoneCallback", start)
body = source[start:end]

assert "if (lastPos < PreCursorTW(tw))" in body
assert "if (PreOverLen(tw) > 0)" in body
assert "if (PreOverStr(tw) == NULL)" in body
assert body.count("PreUnder(tw) = False;") >= 3
assert "return 0;" in body

print("XmText preedit-start snapshot handling is checked")
