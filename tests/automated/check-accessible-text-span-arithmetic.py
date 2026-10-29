#!/usr/bin/env python3
"""Ensure accessibility text ranges are widened before subtraction."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text(
    encoding="utf-8"
)
start = source.index("static char *text_impl(")
end = source.index("static Boolean input_allowed", start)
body = source[start:end]
assert "size_t capacity, span;" in body
assert "span = (size_t)end - (size_t)start;" in body
assert "capacity = span * MB_CUR_MAX + 1;" in body
assert "XmTextGetSubstring(n->widget, start, (int)span" in body
assert "XmTextFieldGetSubstring(n->widget, start, (int)span" in body
assert "(size_t)(end - start)" not in body
print("PASS accessibility text span arithmetic is overflow-safe")
