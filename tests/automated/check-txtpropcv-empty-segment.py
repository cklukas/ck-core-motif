#!/usr/bin/env python3
"""Check separator/tab insertion tolerates empty text segments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("/* Before returning Success")
body = source[start:source.index("return(return_status);", start)]
assert "size_t buffer_length = *buffer ? strlen(*buffer) : 0;" in body
assert "newlength = buffer_length + prefix_length;" in body
assert "if (*buffer != NULL)" in body
assert "newstring[i] = '\\0';" in body
assert "strcpy(&newstring[i], *buffer);" in body
print("PASS empty text segments are safe during separator insertion")
