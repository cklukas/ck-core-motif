#!/usr/bin/env python3
"""Check XmString byte-stream conversion validates component bounds."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmCvtByteStreamToXmString(unsigned char *property)")
end = source.index("\n_XmStringEntry\n_XmStringEntryCopy", start)
region = source[start:end]

assert "size_t remaining;" in region
assert "remaining < (size_t)(ASNTAG + CSSHORTLEN)" in region
assert "malformed = TRUE;" in region
assert "return NULL;" in region
assert "length > remaining - (size_t)_asn1_size(length)" in region

print("XmString byte-stream conversion validates component bounds")
