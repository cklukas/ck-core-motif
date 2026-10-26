#!/usr/bin/env python3
"""Check XmString segment merging cannot wrap its byte-count field."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("if (((a_tag == b_tag)")
body = source[start:source.index("{\n\t  if (b_len)", start)]
assert "b_len <= UINT_MAX - a_len" in body
assert "!_XmEntryOptimized(a_last) || a_len + b_len <= UCHAR_MAX" in body
print("PASS XmString segment merge byte-count bounds")
