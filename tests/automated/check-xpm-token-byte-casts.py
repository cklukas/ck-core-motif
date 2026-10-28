#!/usr/bin/env python3
"""Check XPM token scanners preserve high-bit input bytes explicitly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text()
start = source.index("xpmNextWord(")
region = source[start : source.index("/*\n * skip whitespace and compute", start)]

assert region.count("*buf++ = (char)(unsigned char)c;") == 2
print("XPM token scanners preserve high-bit input bytes explicitly")
