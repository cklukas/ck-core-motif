#!/usr/bin/env python3
"""Check that XmString parser rendition arrays use byte-sized allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static _XmString \n_XmStringNonOptCreate(")
end = source.index("\n/*\n * Converts from ASN.1", start)
region = source[start:end]

assert region.count("sizeof(XmStringTag)") >= 4
assert region.count("if (!new_tags)") >= 2
assert "_XmUnoptSegRendBegins(&seg) = new_tags;" in region
assert "_XmUnoptSegRendEnds(&seg) = new_tags;" in region
print("PASS XmString parser rendition arrays use checked byte sizes")
