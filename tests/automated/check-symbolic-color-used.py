#!/usr/bin/env python3
"""Check ImageCache detects matching XPM symbolic colors."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.rindex("SymbolicColorUsed(String color_name")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "color_name == NULL || xpm_colors == NULL" in function
assert "if (i == ncolors) return False" in function
assert "return True ;" in function
assert function.rindex("return True") > function.index("if (i == ncolors)")
print("PASS symbolic XPM color detection")
