#!/usr/bin/env python3
"""Ensure DragBS readers validate serialized heap offsets."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
assert source.count("propertyRecPtr->info.heap_offset > lengthRtn") == 2
assert "propertyRecPtr->info.heap_offset <\n\t     sizeof(xmMotifAtomsPropertyRec)" in source
assert "propertyRecPtr->info.heap_offset < sizeof(xmMotifTargetsPropertyRec)" in source
print("PASS DragBS property offsets are validated")
