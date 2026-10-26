#!/usr/bin/env python3
"""Check that text-property conversion rejects non-byte formats."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmTextToLocaleText(")
end = source.index("void\n_XmConvertComplete", start)
region = source[start:end]
assert "if (format != 8)" in region
assert "return NULL;" in region
print("locale-text conversion rejects non-8-bit text properties")
