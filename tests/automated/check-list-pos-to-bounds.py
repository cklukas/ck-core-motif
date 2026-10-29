#!/usr/bin/env python3
"""Check XmListPosToBounds clamps returned X/Y positions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("XmListPosToBounds(Widget")
region = source[start:source.index("XmListUpdateSelectedList", start)]

assert "ix = bounded_position((long long)lw->list.BaseX - ht);" in region
assert "iy = bounded_position((long long)LINEHEIGHTS" in region
assert "ix = lw->list.BaseX - ht;" not in region
assert "iy = LINEHEIGHTS(lw," not in region
print("PASS XmListPosToBounds clamps returned positions")
