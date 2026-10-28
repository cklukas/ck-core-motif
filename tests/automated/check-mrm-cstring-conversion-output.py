#!/usr/bin/env python3
"""Ensure Mrm compound-string conversion validates its output pointer."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(
    encoding="utf-8"
)
start = text.index("UrmCWR__AppendCString (URMResourceContextPtr")
body = text[start:text.index("\n}\n", start)]
length_pos = body.index("if ( encoded_len <= 0 )")
guard_pos = body.index("if (asn1 == NULL)")
copy_pos = body.index("memcpy (cstgadr, (char *)asn1, len)")
assert length_pos < guard_pos < copy_pos
print("Mrm compound-string conversion validates its output pointer")
