#!/usr/bin/env python3
"""Check XmText overstrike preedit early-exit cleanup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("static void\nPreeditDone(")
end = source.index("\n\n/*\n * This function shows the correspondence", start)
body = source[start:end]

marker = "if (end_preedit) {\n          if (PreOverMaxLen(tw) != PreOverLen(tw))"
assert marker in body
assert "PreOverStr(tw) = NULL;" in body
assert "PreStartTW(tw) = PreEndTW(tw) = PreCursorTW(tw) = 0;" in body
assert "PreUnder(tw) = False;" in body

print("XmText preedit verify early exit cleans up overstrike state")
