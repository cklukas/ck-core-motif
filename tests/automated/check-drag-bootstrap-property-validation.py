#!/usr/bin/env python3
"""Check drag bootstrap properties reject truncation and null buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
assert source.count("bytesafter == 0") >= 4
assert source.count("property != NULL") >= 2
assert source.count("propertyRecPtr != NULL") >= 2

print("Drag bootstrap properties reject truncation and null buffers")
