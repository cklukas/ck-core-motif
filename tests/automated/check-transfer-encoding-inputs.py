#!/usr/bin/env python3
"""Check encoding-atom lookup rejects a null widget before app lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
start = source.index("XmeGetEncodingAtom")
region = source[start:source.index("_XmTextToLocaleText", start)]
assert "if (w == NULL)" in region
assert region.index("if (w == NULL)") < region.index("_XmWidgetToAppContext(w)")
assert "return None;" in region
print("PASS encoding-atom lookup validates null widgets first")
