#!/usr/bin/env python3
"""Check that mwm bitmap path construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap GetNamedPixmap")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "pathLength < 0 || (size_t)pathLength == SIZE_MAX" in region
assert "fullPathName = (char*) XtMalloc((size_t)pathLength + 1);" in region
assert "if (fullPathName == NULL)" in region
assert "return pixmap;" in region
print("PASS mwm bitmap path allocation is guarded")
