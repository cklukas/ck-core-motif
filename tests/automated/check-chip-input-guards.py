#!/usr/bin/env python3
"""Ensure Chip public actions reject null widget handles before type checks."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text(
    encoding="utf-8"
)
invoke = source[source.index("void XmChipInvoke") : source.index("void XmChipRemove")]
remove_start = source.index("void XmChipRemove")
remove = source[remove_start : source.index("static void Events", remove_start)]
assert "if (widget && XmIsChip(widget)" in invoke
assert "if (widget && XmIsChip(widget)" in remove
print("PASS Chip action APIs validate null widget handles")
