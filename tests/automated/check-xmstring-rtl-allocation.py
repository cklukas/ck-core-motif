#!/usr/bin/env python3
"""Check RTL compound-string rendering handles scratch allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(
    encoding="utf-8"
)
start = source.index("p = flip_char_extra = (char *) ALLOCATE_LOCAL(seg_len)")
end = source.index("draw_text = p;", start)
region = source[start:end]
assert "if (p == NULL)" in region
assert "return;" in region
print("RTL compound-string rendering handles scratch allocation failure")
