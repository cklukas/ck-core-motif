#!/usr/bin/env python3
"""Check ASN.1 header bounds before Motif length decoding."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert text.count("remaining < 4") >= 2
assert text.count("remaining < 6") >= 2
assert "value[offset + 3] & 0x80" in text
print("compound string header bounds contract passed")
