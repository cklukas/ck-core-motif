#!/usr/bin/env python3
"""Ensure Notebook page/scroller coordinates do not wrap on narrow pages."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nLayoutPages")
end = source.index("static void\nLayoutMajorTabs", start)
body = source[start:end]
assert "int x, y, x1, y1, x2;" in body
assert "Dimension x, y, x1, y1, x2;" not in body
assert "x2 = x + nb->notebook.page_width - nb->notebook.scroller_width;" in body
print("PASS Notebook page and scroller coordinates use signed arithmetic")
