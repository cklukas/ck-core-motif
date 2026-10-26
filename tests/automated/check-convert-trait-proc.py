#!/usr/bin/env python3
"""Check that conversion dispatch validates the trait function pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertHandler(Widget wid")
end = source.index("\n/* DragConvertHandler", start)
region = source[start:end]
assert "ttrait != NULL && ttrait -> convertProc != NULL" in region
assert region.index("convertProc != NULL") < region.index("ttrait -> convertProc(wid")
print("conversion dispatch skips traits without a convert procedure")
