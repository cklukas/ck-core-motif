#!/usr/bin/env python3
"""Check Text modify-verify callback conversions remain width-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmTextModifyVerify(")
end = source.index("static XmTextStatus", start)
body = source[start:end]

assert "(size_t)block->length" in body
assert "(size_t)tvcb.text->length" in body
assert "XtMalloc((Cardinal)wcs_capacity)" in body
assert "converted > (size_t)INT_MAX" in body
assert "wcs_newblock.length = (int)converted;" in body

print("Text callback conversion widths are explicit")
