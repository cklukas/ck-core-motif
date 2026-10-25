#!/usr/bin/env python3
"""Check XmString byte-stream conversion bounds its output allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmCvtXmStringToByteStream(")
end = source.index("Dimension \nXmStringBaseline", start)
region = source[start:end]
assert "component_size > UINT_MAX - length" in region
assert "component_size + length > UINT_MAX - len" in region
assert "component_size = _calc_header_size(len);" in region
assert "if (ext == NULL)" in region
assert region.index("if (ext == NULL)") < region.index("*prop_return = ext;")
print("PASS XmString byte-stream conversion handles allocation failure")
