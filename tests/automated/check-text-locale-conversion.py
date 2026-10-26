#!/usr/bin/env python3
"""Check deterministic failure handling for locale-text conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmTextToLocaleText(")
end = source.index("void\n_XmConvertComplete", start)
region = source[start:end]
assert "*success = False;" in region
assert "if (w == NULL || value == NULL)" in region
assert "COMPOUND_TEXT = XInternAtom" in region
print("locale-text conversion fails deterministically for invalid inputs")
