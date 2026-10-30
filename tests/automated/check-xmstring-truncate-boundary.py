#!/usr/bin/env python3
"""Check ASN.1 truncation never reads a component past the string end."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("_XmStringTruncateASN1(unsigned char *str, int n)")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (ap < a_end)" in body
assert "if (ap >= a_end)" in body
assert "size_t remaining = (size_t)(a_end - ap);" in body
assert "(size_t)length > remaining - (size_t)header" in body
assert body.index("if (ap >= a_end)") < body.index("length = _read_asn1_length(ap);", body.index("if (ap >= a_end)"))

print("XmString ASN.1 truncation is boundary-safe")
