#!/usr/bin/env python3
"""Ensure deferred clipboard callbacks validate payload conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("ClipboardCallback(Widget w")
end = source.index("\n/*", start)
body = source[start:end]

assert "format == 32" in body
assert "size > ULONG_MAX / unit" in body
assert "(size != 0 && value == NULL)" in body
assert "size = BYTELENGTH( size, format );" in body

print("PASS deferred clipboard callback payload conversion is bounded")
