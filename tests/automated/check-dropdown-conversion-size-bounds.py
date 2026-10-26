#!/usr/bin/env python3
"""Check DropDown bounds the XrmValue conversion size before assignment."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nSetListFromText")
end = source.index("\n/*", start + 1)
region = source[start:end]

assert "if (text_length >= UINT_MAX)" in region
assert "XtFree(ptr);" in region
assert "from.size = (unsigned int)(text_length + 1);" in region
print("PASS DropDown conversion size is bounded before XrmValue assignment")
