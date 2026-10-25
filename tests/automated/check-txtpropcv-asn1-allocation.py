#!/usr/bin/env python3
"""Check ASN.1 compound-string output allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("case XmSTYLE_COMPOUND_STRING:")
body = source[start:source.index("case XmSTYLE_LOCALE:", start)]
assert "text_prop_return->value = ubufptr =" in body
assert "if (text_prop_return->value == NULL)" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS ASN.1 compound-string output allocation is guarded")
