#!/usr/bin/env python3
"""Check XmString byte-stream length validates null input and arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmStringByteStreamLength(")
end = source.index("/*", start)
region = source[start:end]
assert "if (string == NULL)" in region
assert "header_size = _calc_header_size(len);" in region
assert "header_size > UINT_MAX - len" in region
assert region.index("if (string == NULL)") < region.index("_read_string_length")
print("PASS XmString byte-stream length validation")
