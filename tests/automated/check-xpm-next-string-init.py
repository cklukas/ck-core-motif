#!/usr/bin/env python3
"""Check XPM buffer-string scanning initializes its lookahead character."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text()
start = source.index("xpmNextString(xpmData *data)")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "register char c = '\\0';" in function
assert "if (c != '\\0')" in function
print("PASS XPM next-string lookahead initialization")
