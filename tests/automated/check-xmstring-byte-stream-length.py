#!/usr/bin/env python3
"""Check XmStringByteStreamLength validates ASN.1 input."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmStringByteStreamLength(unsigned char *string)")
end = source.index("\n}\n", start)
region = source[start:end]

assert "if (!_is_asn1(string))" in region
assert "return(0);" in region
assert region.index("_is_asn1") < region.index("_read_string_length")

print("XmStringByteStreamLength validates ASN.1 input")
