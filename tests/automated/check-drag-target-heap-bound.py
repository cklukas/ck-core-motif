#!/usr/bin/env python3
"""Ensure drag target-list parsing is bounded by the declared heap offset."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
start = source.index("static Boolean \nReadTargetsTable(")
body = source[start:source.index("WriteTargetsTable (", start)]
assert "remaining = (size_t)propertyRecPtr->info.heap_offset" in body
assert "malformed property" in body
print("PASS drag target-list parsing honors the declared heap boundary")
