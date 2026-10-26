#!/usr/bin/env python3
"""Check SpinBox numeric formatting handles allocation failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SpinB.c").read_text()
start = source.index("NumToString(char **buffer")
body = source[start:source.index("/******************************************************************************\n * DrawSpinArrow", start)]
assert "*buffer = NULL" in body
assert "allocation_size > (size_t)UINT_MAX" in body
assert "if (*buffer == NULL)" in body
print("PASS SpinBox numeric formatter allocation handling")
