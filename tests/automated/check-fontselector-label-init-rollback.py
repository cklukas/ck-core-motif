#!/usr/bin/env python3
"""Check FontSelector initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
start = source.index("CopyFontSelectorString")
end = source.index("    ENCODING_STRING(fsw)", start)
region = source[start:end]

assert "return copy != NULL ? copy : XmStringCreateLocalized(\"\");" in region
assert region.count("CopyFontSelectorString(") >= 19
assert "XmStringCopy(ANY_STRING(fsw))" not in region
assert "XmStringCopy(XLFD_STRING(fsw))" not in region
print("FontSelector labels handle initialization allocation failure")
