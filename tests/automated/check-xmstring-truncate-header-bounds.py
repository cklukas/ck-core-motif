#!/usr/bin/env python3
"""Ensure ASN.1 truncation checks component headers before reading lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("_XmStringTruncateASN1")
end = source.index("\n}\n", start) + 3
region = source[start:end]

assert region.count("remaining < (size_t)(ASNTAG + CSSHORTLEN)") >= 2
assert region.count("_is_asn1_long(ap)") >= 2
assert region.count("length = _read_asn1_length(ap);") >= 2
print("PASS ASN.1 truncation checks component header bounds")
