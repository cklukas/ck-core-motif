#!/usr/bin/env python3
"""Check DragUnder geometry narrowing rejects Position overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragUnder.c").read_text()

assert "#include <limits.h>" in source
assert "AddDragUnderOffset" in source
assert "value < SHRT_MIN || value > SHRT_MAX" in source
assert source.count("!AddDragUnderOffset(extents.x, offset, &x)") == 2
assert source.count("!AddDragUnderOffset(extents.y, offset, &y)") == 2
assert "pData->height = (Dimension)(height - (*thickness << 1));" in source

print("DragUnder geometry narrowing is range checked")
