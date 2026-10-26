#!/usr/bin/env python3
"""Ensure transfer callbacks handle missing conversion contexts."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void \nSecondaryDone(")
body = source[start:source.index("/****************************************************************/\n/* XmeClipboardSource", start)]
assert "if (cc == NULL)" in body
assert "XtDisownSelection" in body
callback = source[source.index("static void \nClipboardCallback") :]
assert "cc == NULL ||" in callback
print("PASS transfer callbacks guard missing conversion contexts")
