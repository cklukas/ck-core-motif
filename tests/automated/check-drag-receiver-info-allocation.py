#!/usr/bin/env python3
"""Ensure drag receiver-info growth checks overflow and allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("XmDragReceiverInfo\n_XmAllocReceiverInfo(")
body = source[start:source.index("/* Find a window", start)]
assert "old_max > (UINT_MAX - 2) / 2" in body
assert "UINT_MAX /" in body
assert "if (new_infos == NULL)" in body
assert "return NULL;" in body
print("PASS drag receiver-info growth checks overflow and allocation failure")
