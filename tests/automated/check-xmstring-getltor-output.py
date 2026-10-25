#!/usr/bin/env python3
"""Check that XmStringGetLtoR clears output before input validation."""

from pathlib import Path


source = Path("lib/Xm/XmStringGet.c").read_text()
start = source.index("XmStringGetLtoR(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (text == NULL)" in function
assert "*text = NULL;" in function
assert "if (!string || !tag)" in function
assert function.index("*text = NULL;") < function.index("if (!string || !tag)")

print("XmStringGetLtoR clears output on invalid input")
