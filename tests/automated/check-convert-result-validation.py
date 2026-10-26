#!/usr/bin/env python3
"""Check that the shared conversion boundary rejects malformed results."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertHandler(Widget wid")
end = source.index("\n/* DragConvertHandler", start)
region = source[start:end]
assert "cbstruct.format != 8" in region
assert "cbstruct.length != 0 && cbstruct.value == NULL" in region
assert region.index("cbstruct.format != 8") < region.index("/* Copy out the data */")
print("shared conversion boundary rejects invalid format and null payload")
