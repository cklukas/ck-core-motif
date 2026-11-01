#!/usr/bin/env python3
"""Check XmString byte-stream conversion validates component bounds."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmCvtByteStreamToXmString(unsigned char *property)")
end = source.index("\n_XmStringEntry\n_XmStringEntryCopy", start)
region = source[start:end]

def function_region(signature, next_signature=None):
    begin = source.index(signature)
    finish = source.index(next_signature, begin) if next_signature else len(source)
    return source[begin:finish]

optimized = function_region("_XmStringOptCreate(", "static void\nfinish_segment")
unoptimized = function_region("_XmStringNonOptCreate(", "/*\n * Converts from ASN.1")
truncate = function_region("_XmStringTruncateASN1(")

assert "size_t remaining;" in region
assert "remaining < (size_t)(ASNTAG + CSSHORTLEN)" in region
assert "malformed = TRUE;" in region
assert "return NULL;" in region
assert "component_header = _read_length(c_opt);" in region
assert "length > remaining - (size_t)component_header" in region
assert "c_opt += length + component_header;" in region
for parser in (optimized, unoptimized, region, truncate):
    assert "_asn1_size(length)" not in parser
    assert "_read_length(" in parser

print("XmString byte-stream conversion uses encoded component-header widths")
