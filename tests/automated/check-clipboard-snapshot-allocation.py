#!/usr/bin/env python3
"""Ensure deferred clipboard snapshot allocations are checked."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("Boolean\nXmeClipboardSource(")
body = source[start:source.index("static void \nClipboardCallback", start)]
assert "if (DataIdDictionary == NULL)" in body
assert "if (req == NULL)" in body
assert "XmClipboardEndCopy" in body
print("PASS deferred clipboard snapshot allocations are checked")
