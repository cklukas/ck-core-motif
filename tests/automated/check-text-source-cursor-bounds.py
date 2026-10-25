#!/usr/bin/env python3
"""Check large Text source conversions use size-aware bounded cursors."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmStringSourceGetString(")
end = source.index("/********************************************************", start)
region = source[start:end]
assert "size_t destpos;" in region
assert "allocation - 1 - destpos" in region
assert "allocation / sizeof(wchar_t)" in region
assert "return_val == (size_t)-1" in region
assert "XtFree(buf);" in region
print("PASS Text source conversion bounds large cursors")
