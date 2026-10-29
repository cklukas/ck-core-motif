#!/usr/bin/env python3
"""Ensure Column flush-fill geometry cannot wrap a negative inner height."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Column.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nHorizontalLayout")
end = source.index("static void\nVerticalLayout", start)
body = source[start:end]
assert "int\t\tfill_height;" in body
assert "fill_height = col_height - 2 *" in body
assert "fillHeight = fill_height > 0 ? (Dimension)fill_height : 1;" in body
assert "fillHeight = col_height - 2 *" not in body
print("PASS Column flush-fill height is bounded before Dimension conversion")
