#!/usr/bin/env python3
"""Check TextOut resize measurement validates source progress."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
start = source.index("TextFindNewWidth")
end = source.index("\n\nstatic void \nCheckForNewSize", start)
body = source[start:end]

assert body.count("next_position <= linestart") >= 4
assert body.count("block.length > 0 && block.ptr == NULL") >= 4
assert "*widthRtn = 0;" in body
assert "*heightRtn = 0;" in body

print("TextOut resize measurement guards source progress")
