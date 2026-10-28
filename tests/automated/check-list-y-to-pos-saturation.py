#!/usr/bin/env python3
"""Check List Y-to-position conversion cannot overflow its out-of-range marker."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
which_start = source.index("static int\nWhichItem")
which_end = source.index("static void\nListConvert", which_start)
which_body = source[which_start:which_end]
y_start = source.index("int\nXmListYToPos")
y_end = source.index("Boolean\nXmListPosToBounds", y_start)
y_body = source[y_start:y_end]

assert "lw->list.itemCount == INT_MAX" in which_body
assert "? INT_MAX : lw->list.itemCount + 1" in which_body
assert "pos = WhichItem(lw, y);" in y_body
assert "if (pos < INT_MAX)" in y_body
print("List Y-to-position conversion saturates safely")
