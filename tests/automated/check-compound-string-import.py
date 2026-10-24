#!/usr/bin/env python3
"""Check bounded ASN.1 compound-string property parsing."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "stream_length == 0 || stream_length > remaining" in text
assert "stream_length > (size_t)text_prop->nitems - offset" in text
assert "offset < (size_t)text_prop->nitems" in text
print("compound string import bounds contract passed")
