#!/usr/bin/env python3
"""Check TextField insertion bounds locale-converted string lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void\nInsertSelection")
end = source.index("\n/* ARGSUSED */\nstatic void\nHandleInsertTargets", start)
body = source[start:end]

assert "size_t len = strlen(total_value);" in body
assert "if (len > (size_t)INT_MAX)" in body
assert "num_chars = (int)len;" in body
assert "len > (size_t)INT_MAX - 1" in body
assert "len++;" in body

print("TextField locale-converted insertion lengths are bounded")
