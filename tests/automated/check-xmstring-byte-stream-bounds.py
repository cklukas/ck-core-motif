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

component = function_region("_check_component(", "/*\n * Private routines for reading/writing")

# every component, in both passes, goes through the one bounds check
assert "size_t remaining;" in component
assert "remaining < (size_t)(ASNTAG + CSSHORTLEN)" in component
assert "(size_t)*length > remaining - (size_t)*header" in component
assert "_check_component(c_opt, end, &length, &component_header)" in region
assert "_check_component(c, end, &length, &header)" in unoptimized
assert "goto conversion_failed;" in unoptimized
assert "malformed = TRUE;" in region
assert "return NULL;" in region
assert "c_opt += length + component_header;" in region
for parser in (optimized, unoptimized, region, truncate):
    assert "_asn1_size(length)" not in parser
assert "_read_length(" in optimized and "_read_length(" in truncate
assert "_read_length(" in component

print("XmString byte-stream conversion uses encoded component-header widths")
