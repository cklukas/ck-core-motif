#!/usr/bin/env python3
"""Ensure deferred clipboard callbacks guard missing snapshot records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void \nClipboardCallback(")
body = source[start:source.index("/* Warning routine */", start)]
assert "data_id == NULL || DataIdDictionary == NULL" in body
assert "if (req == NULL)" in body
assert "req -> outstanding--" in body
print("PASS clipboard callbacks guard missing snapshot records")
