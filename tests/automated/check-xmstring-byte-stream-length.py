#!/usr/bin/env python3
"""Check XmString byte-stream length validates input and arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringByteStreamLength(")
end = source.index("/*\n * build the ASN.1 format", start)
region = source[start:end]
assert "if (string == NULL)" in region
assert "unsigned int len, header_size;" in region
assert "header_size > UINT_MAX - len" in region
assert region.index("if (string == NULL)") < region.index("_XmProcessLock")
print("PASS XmString byte-stream length validation is guarded")
