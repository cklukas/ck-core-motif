#!/usr/bin/env python3
"""Check that locale-text conversion bounds input length before Xlib."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmTextToLocaleText(")
end = source.index("void\n_XmConvertComplete", start)
region = source[start:end]
assert "length > (unsigned long)INT_MAX" in region
assert "status = XmbTextPropertyToTextList" in region
print("locale-text conversion bounds length before Xlib decoding")
