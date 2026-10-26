#!/usr/bin/env python3
"""Check that drag-source target counts are bounded before Xt resource use."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDragSource(")
end = source.index("/****************************************************************/\n/* Destination section", start)
region = source[start:end]
assert "size > (unsigned long)UINT_MAX" in region
assert "XmNnumExportTargets, (Cardinal)size" in region
print("drag-source target counts are bounded before Cardinal conversion")
