#!/usr/bin/env python3
"""Check COMPOUND_STRING export rejects zero-sized ASN.1 conversions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("case XmSTYLE_COMPOUND_STRING:")
end = source.index("      /* Allocate that amount", start)
body = source[start:end]

assert "item_size <= 0" in body
assert "XmCvtXmStringToByteStream(string_table[i],NULL)" in body
assert "return BadAlloc;" in body

print("compound-string export rejects zero-sized ASN.1 conversion")
