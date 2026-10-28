#!/usr/bin/env python3
"""Check path normalization guards an empty constructed path."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("static String\nGetQualifiedDir")
body = source[start:source.index("\n}\n", start) + 3]

assert "userDirLen == 0 || outputBuf[userDirLen - 1]" in body

print("Path normalization guards empty constructed paths")
