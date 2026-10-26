#!/usr/bin/env python3
"""Check FileSB handles failed qualified-path construction before strlen."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
marker = "_XmOSQualifyFileSpec( dirPartPtr, patternPartPtr"
start = source.index(marker)
body = source[start:source.index("qDirLen = strlen", start)]
assert "qualifiedDir == NULL || qualifiedPattern == NULL" in body
assert "XtFree( qualifiedDir)" in body
assert "return ;" in body
print("PASS FileSB qualified-path failure handling")
